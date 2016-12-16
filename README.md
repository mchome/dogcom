# dogcom ![travis-ci](https://travis-ci.org/mchome/dogcom.svg "Build status")
[Drcom-generic](https://github.com/drcoms/drcom-generic) implementation in C.

```
Usage:
        dogcom -m <dhcp/pppoe> -c <FILEPATH> [options <argument>]...

Options:
        --mode <dhcp/pppoe>, -m <dhcp/pppoe>  set your dogcom mode
        --conf <FILEPATH>, -c <FILEPATH>      import configuration file
        --log <LOGPATH>, -l <LOGPATH>         specify log file
        --verbose, -v                         set verbose flag
        --help, -h                            display this help
```

Config file is compatible with [drcom-generic](https://github.com/drcoms/drcom-generic).

#### Example:

    $ dogcom -m dhcp -c dogcom.conf

#### To build:

```bash
$ make # Linux
$ make win32=y # Windows(MinGW)
```

### Thanks:
- [gdut-drcom](https://github.com/chenhaowen01/gdut-drcom 'chenhaowen01')
- [jlu-drcom-client](https://github.com/drcoms/jlu-drcom-client/tree/master/C-version 'feix')

### Special thanks:
- [Drcom-generic](https://github.com/drcoms/drcom-generic 'ly0')

### License:
![AGPL V3](https://cloud.githubusercontent.com/assets/7392658/20011165/a0caabdc-a2e5-11e6-974c-8d4961c7d6d3.png)
