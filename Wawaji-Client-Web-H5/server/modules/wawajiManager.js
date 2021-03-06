var Signal = require('../vendor/signal.js');
var vault = require('./vault');
var SignalingToken = require('./token');
const WebSocket = require('ws');
const WawajiStatus = require('./constants').WawajiStatus;
const StreamMethod = require('./constants').StreamMethod;
const JsmpegStream = require('./jsmpegStream');
const ImageStream = require('./imageStream');
const request = require('request');
const Utils = require('./utils');

var global_port = 8100;

var debug = true;

var dbg = function () {
    if (debug) {
        var x = [];
        for (var i in arguments) x.push(arguments[i]);
        console.log.apply(null, ['Agora sig client dbg :'].concat(x));
    }
};

Wawaji = {};

Wawaji.Server = function (serverid, io) {
    //utils
    var client = this;

    /*------------------------------------------------
    |   Session API : events
    \*----------------------------------------------*/
    this.onStarted = null;
    this.onStartFailed = null;

    /*------------------------------------------------
    |   Session API : initiate
    \*----------------------------------------------*/
    //begin creating singal
    var signal = Signal(vault.appid);
    var cc_name = "wawaji_cc_" + serverid;
    dbg(`login as ${cc_name}`)
    this.session = signal.login(cc_name, SignalingToken.get(vault.appid, vault.appcert, "wawaji_cc_" + serverid, 1));
    this.uid = null;
    this.channel = null;
    this.ipaddress = Utils.getIp();


    this.session.onLoginSuccess = function (uid) {
        dbg("login successful " + uid);
        client.uid = uid;
        client.machines = new Wawaji.MachineSet();
        client.onStarted && client.onStarted();
    };

    //if fail
    this.session.onLoginFailed = function () {
        dbg("login failed ");
        client.onStartFailed && client.onStartFailed();
    };

    this.session.onMessageInstantReceive = function (account, uid, msg) {
        dbg("msg received from " + account + ": " + msg);
        var data = JSON.parse(msg);
        var machine = null;
        var response = null;

        if (data.type !== undefined) {
            switch (data.type) {
                case "LIST":
                    response = { type: "LIST", machines: client.machines.toJSON() }
                    client.session.messageInstantSend(account, JSON.stringify(response));
                    break;
                case "PLAY":
                    machine = client.machines.get(data.machine);
                    machine && machine.play(account);
                    break;
            }
        }
    }

    /*------------------------------------------------
    |   Class : Machines
    \*----------------------------------------------*/
    Wawaji.MachineSet = function () {
        var collection = this;
        this.__machines = [];
        this.add = function (name, url) {
            collection.__machines.push(new Wawaji.Machine(name, url));
            dbg("machine " + name + " added");
            return collection;
        }

        this.get = function (name) {
            for (var i = 0; i < collection.__machines.length; i++) {
                var machine = collection.__machines[i];
                if (name === machine.name) {
                    return machine;
                }
            }
            return null;
        }

        this.toJSON = function () {
            var results = [];
            dbg(`toJSON ${collection.__machines.length} machines`);
            for (var i = 0; i < collection.__machines.length; i++) {
                var machine = collection.__machines[i];
                if (machine.online) {
                    results.push({ name: machine.name, status: machine.status, players: [], video_channel: machine.video_channel, video_host: machine.video_host, video_appid: machine.video_appid, video_rotation: machine.video_rotation, stream_method: machine.stream_method });
                }
            }
            return results;
        }
    }



    Wawaji.User = function (uid, account) {
        this.uid = uid;
        this.account = account;
    }


    /*------------------------------------------------
    |   Class : Machine
    \*----------------------------------------------*/
    Wawaji.Machine = function (name, profile) {
        var machine = this;
        this.status = WawajiStatus.INITIAL;
        this.name = "wawaji_" + name;
        machine.profile = profile;

        var signal = Signal(vault.appid);
        this.session = signal.login(this.name, SignalingToken.get(vault.appid, vault.appcert, this.name, 1));
        this.uid = null;
        this.channel = null;
        this.users = [];
        this.queue = [];
        this.socket = null;
        this.url = profile.url;
        this.playing = null;
        this.video_channel = profile.video_channel;
        this.video_host = profile.video_host;
        this.video_appid = profile.appid;
        this.video_rotation = profile.video_rotation;
        this.result = false;
        this.prepare_timer = null;
        this.game_timer = null;
        this.result_timer = null;
        this.stream_method = profile.mode;
        this.stream_port1 = global_port++;
        this.stream_port2 = global_port++;
        this.websocket_port1 = global_port++;
        this.websocket_port2 = global_port++;
        this.online = false;
        // this.attributes = { queue: [], playing: null, cameras: { front: this.websocket_port1, back: this.websocket_port2 } };


        dbg(`stream: ${machine.stream_port1}, ${machine.stream_port2} ws: ${machine.websocket_port1},${machine.websocket_port2}`);
        if (this.stream_method === StreamMethod.JSMPEG) {
            if (profile.appcert) {
                request(`http://recording.agorapremium.agora.io:9001/agora/media/genDynamicKey5?uid=0&key=${profile.appid}&sign=${profile.appcert}&channelname=${profile.video_channel}`, function (err, response, body) {
                    machine.stream = new JsmpegStream(machine.stream_port1, machine.stream_port2, machine.websocket_port1, machine.websocket_port2, profile.stream_secret, profile.appid, profile.video_channel, body);
                });
            } else {
                machine.stream = new JsmpegStream(machine.stream_port1, machine.stream_port2, machine.websocket_port1, machine.websocket_port2, profile.stream_secret, profile.appid, profile.video_channel);
            }
        } else {
            if (!profile.video_host) {
                machine.stream = new ImageStream(io, profile.appid, profile.video_channel, null, "1", "2");
            }
        }



        this.setStatus = function (status, reason) {
            dbg(`[DEBUG] ${machine.name} set to status ${status}: ${reason}`);
            machine.status = status;
        }


        initWS();

        function initWS(cb) {
            if (machine.socket) {
                cb && cb();
                return;
            }
            if (machine.url) {
                var socket = new WebSocket(machine.url);
                machine.socket = socket;

                //need reinitiate websocket link, get current status
                var previous_status = machine.status;
                machine.setStatus(WawajiStatus.INITIALIZING);

                machine.profile.onInit(machine, function () {
                    if (previous_status === WawajiStatus.INITIAL) {
                        machine.setStatus(WawajiStatus.READY);
                    } else {
                        dbg(`restore status successful ${machine.status}`)
                        machine.setStatus(previous_status);
                    }
                    cb && cb();
                });

                machine.socket.onclose = function (e) {
                    dbg("WebSocket closed for " + machine.name);
                    machine.socket = null;
                }
            }
        }

        this.session.onLoginSuccess = function (uid) {
            dbg("login successful " + uid);
            machine.uid = uid;
            machine.channel = machine.session.channelJoin("room_" + machine.name);
            machine.channel.onChannelJoined = function () {
                dbg(`[DEBUG] ${machine.name} is now online`);
                machine.online = true;
                machine.channel.channelClearAttr();
                machine.updateAttrs();
            };

            machine.channel.onChannelJoinFailed = function (ecode) {
                dbg("machine connect failed");
            };

            machine.channel.onChannelUserJoined = function (account, uid) {
                machine.users.push(new Wawaji.User(uid, account));
                dbg(account + " joined, " + machine.users.length + " players in " + machine.name);
            };

            machine.channel.onChannelUserLeaved = function (account, uid) {
                machine.dequeuePlayer(account);
                machine.users = machine.users.filter(function (item) {
                    return item.uid !== uid;
                });
                machine.updateAttrs();
                dbg(account + " leaved, " + machine.users.length + " players in " + machine.name);
            };

            machine.channel.onChannelUserList = function (users) {
                var results = [];
                for (var i = 0; i < users.length; i++) {
                    if (users[i][0] === machine.name) {
                        continue;
                    }
                    results.push(new Wawaji.User(users[i][1], users[i][0]));
                }
                machine.users = results;
                dbg(machine.users.length + " players in " + machine.name);
            };

            machine.channel.onMessageChannelReceive = function (account, uid, msg) {
                dbg("msg received from " + account + ": " + msg);
                var data = JSON.parse(msg);

                if (data.type === "PLAY") {
                    dbg(`player ${account} trying to play`);
                    machine.play(account);
                } else {
                    if (account !== machine.playing) {
                        dbg("channel msg received from not playing user " + account + ": " + msg);
                        return;
                    }
                    if (data && data.type) {
                        if (data.type === "CONTROL") {
                            machine.control(data);
                        } else if (data.type === "CATCH") {
                            machine.catch();
                        }
                    }
                }
            }
        };

        //if fail
        this.session.onLoginFailed = function () {
            dbg("machine login failed");
        };

        this.session.onMessageInstantReceive = function (account, uid, msg) {
            if (account !== machine.playing) {
                dbg("instant msg received from not playing user " + account + ": " + msg);
                return;
            }
            dbg("msg received from " + account + ": " + msg);
            var data = JSON.parse(msg);

            if (data && data.type) {
                if (data.type === "START") {
                    //clear timer
                    clearTimeout(machine.prepare_timer);
                    machine.prepare_timer = null;
                    dbg(`response received from ${account}, start play!`);
                    machine.setStatus(WawajiStatus.READY);
                    machine.playgame(account);
                    machine.sendInfo(account, "START");
                }
            }
        }

        this.play = function (account) {
            dbg("machine status: " + machine.status);
            if (machine.playing === account) {
                dbg("you are already playing");
                machine.sendInfo(account, "PLAYER_ALREADY_PLAYING");
                return false;
            }
            if (machine.queue.indexOf(account) !== -1) {
                machine.sendInfo(account, "PLAYER_ALREADY_IN_QUEUE");
                dbg("you are already in queue");
            }
            if (machine.canPlay()) {
                machine.playgame(account);
                return true;
            } else {
                machine.queuePlayer(account);
                machine.updateAttrs();
                //try to process queue to avoid blockage
                machine.processQueue();
            }
            return false;
        };

        this.playgame = function (account) {
            if (machine.status !== WawajiStatus.READY) {
                dbg(`[ERROR] try to start a game whose machine is not ready, current status is ${machine.status}`);
                return;
            }
            dbg("[DEBUG] about to start a game for " + account);

            machine.setStatus(WawajiStatus.PLAY);
            machine.setPlaying(account);
            machine.updateAttrs();
            machine.game_timer = setTimeout(function () {
                machine.catch();
            }, 30000);
            initWS(function () {
                machine.profile.onPlay(account);
            });
        }

        this.catch = function () {
            if (machine.status !== WawajiStatus.PLAY) {
                dbg(`[ERROR] try to do catch while a machine is not in Play status, current status is ${machine.status}`);
                return;
            }
            machine.setStatus(WawajiStatus.BUSY);
            clearTimeout(machine.game_timer);
            machine.game_timer = null;
            machine.result_timer = setTimeout(function () {
                //machine not returning result, we force return a result then
                machine.onResult(false);
            }, 20 * 1000);
            initWS(function () { machine.profile.onCatch() });
        }

        this.control = function (data) {
            if (machine.status !== WawajiStatus.PLAY) {
                dbg(`[ERROR] try to do catch while a machine is not in Play status, current status is ${machine.status}`);
                return;
            }
            initWS(function () { machine.profile.onControl(data) });
        }

        this.processQueue = function () {
            //can process
            dbg(`[DEBUG] process when status is ${machine.status} and machine.playing is ${machine.playing}`)
            if ((machine.status === WawajiStatus.READY && machine.playing === null) || machine.status === WawajiStatus.WAITING) {
                var player = machine.nextPlayer();
                machine.updateAttrs();
                dbg("try to start next play: " + player);
                if (player) {
                    machine.session.messageInstantSend(player, JSON.stringify({ type: "PREPARE" }));
                    machine.prepare_timer = setTimeout(function () {
                        //wait for 10 seconds and next player if no response
                        machine.prepare_timer = null;
                        machine.sendInfo(player, "KICKED_NO_RESPONSE");
                        dbg("no response, next");
                        machine.status === WawajiStatus.WAITING
                        machine.processQueue();
                    }, 10 * 1000);
                } else {
                    machine.setStatus(WawajiStatus.READY);
                }
            }
        }

        profile.onError = function () {
            machine.session.messageInstantSend(machine.playing, JSON.stringify({ type: "ERROR", data: "UNKNOWN_ERR" }))
            dbg(`[ERROR] Error occured for ${machine.name}, try to restore...`);
            machine.setStatus(WawajiStatus.WAITING);
            machine.processQueue();
        }

        profile.onResult = function (result) {
            clearTimeout(machine.result_timer);
            machine.result_timer = null;
            machine.channel.messageChannelSend(JSON.stringify({ type: "RESULT", data: result, player: machine.playing }));
            machine.setStatus(WawajiStatus.WAITING);
            machine.processQueue();
        }

        this.sendInfo = function (account, m) {
            dbg("sending info " + m);
            machine.session.messageInstantSend(account, JSON.stringify({ type: "INFO", data: m }))
        }

        this.canPlay = function (account) {
            return machine.status === WawajiStatus.READY && machine.queue.length === 0;
        };

        this.setPlaying = function (account) {
            dbg("now playing set to " + account);
            machine.playing = account;
        }

        this.emptyQueue = function () {
            dbg("empty queue");
            machine.queue = [];
        }

        this.queuePlayer = function (account) {
            dbg("put " + account + " in queue");
            if (machine.queue.indexOf(account) !== -1) {
                dbg(account + " already in queue");
                return
            }
            machine.queue.push(account);
        }

        this.dequeuePlayer = function (account) {
            dbg("remove " + account + " from queue");
            machine.queue = machine.queue.filter(function (item) {
                return item.account !== account;
            });
        }

        this.nextPlayer = function () {
            var player = machine.queue.shift();
            player && dbg("queue next player " + player);
            machine.playing = player;
            return player;
        }

        this.updateAttrs = function () {
            var attrs = { queue: machine.queue || [], playing: machine.playing || null, cameras: { front: `ws://${client.ipaddress}:${machine.websocket_port1}`, back: `ws://${client.ipaddress}:${machine.websocket_port2}` } };

            dbg(`[DEBUG] update attributes ${JSON.stringify(attrs)}`);
            machine.channel && machine.channel.channelSetAttr("attrs", JSON.stringify(attrs));
        }
    }
}

module.exports = Wawaji.Server;