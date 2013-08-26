#!/usr/bin/nodejs

var spawn = require('child_process').spawn,
    // Default values
    args = {
        command: false,
        ip: false,
        build: 'build-naocar'
    };

parseArguments();

getNaoIP(function(ip) {
    if (ip) {
        if (!args.command || args.command === 'ssh') {
            console.log('ssh nao@' + ip);
        } else if (args.command === 'sendServer') {
            console.log('scp '+args.build+'/sdk/bin/RemoteServerModule nao@'+ip+':/home/nao/nao-car/');
        }
    } else {
        process.exit(1);
    }
});

function parseArguments() {
    var i, l, sepPos;

    for (i = 2, l = process.argv.length; i < l; i += 1) {
        if ((sepPos = process.argv[i].indexOf('=')) !== -1) {
            args[process.argv[i].substr(0, sepPos)] = process.argv[i].substr(sepPos + 1);
        } else {
            args.command = process.argv[i];
        }
    }
}

function getNaoIP(callback) {
    if (args.ip) {
        callback(args.ip);
        return;
    }
    var browser = spawn('avahi-browse', ['-r', '_naoqi._tcp']),
        timer = setTimeout(function() { browser.kill(); callback(false); }, 1000);
    browser.stdout.on('data', function(stdout) {
        var data = stdout.toString(),
            regex = /\s*address\s*=\s*\[(([0-9\.])+)\]/,
            lines,
            i, l,
            matches;
        if (data.indexOf('address') !== -1) {
            lines = data.split("\n");
            for (i = 0, l = lines.length; i < l; i += 1) {
                matches = regex.exec(lines[i]);
                if (matches) {
                    clearTimeout(timer);
                    callback(matches[1]);
                    browser.kill();
                    return;
                }
            }
        }
    });
};
