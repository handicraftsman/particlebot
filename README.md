# particlebot
An IRC bot in C++

## Building

```bash
$ mkdir build && cd build
$ meson ..
$ ninja
```

## Configuring
A sample config file:

```json
{
  "servers": {
    "freenode": {
      "host": "irc.freenode.net",
      "nick": "nickname",
      "pass": "password",
      "autojoin": [
        "#a-random-channel"
      ],
      "autoowner": "your_hostname"
    },
  },
  "plugins": [
    "chanop",
    "fun"
  ],
  "lua-plugins": [
    "eval",
    "titler"
  ],
  "prefix": "@"
}
```

## Starting

```bash
$ ./particlebot -c /path/to/config.json -l info # all log levels can be seen in `./particlebot --help`
```
