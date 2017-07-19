\anchor MLog
[TOC]

Milo Code DB main ([online](https://qtdocs.milosolutions.com/milo-code-db/main/) | [offline](\ref milodatabasemain))

# Description # {#description}

MLog is the reference logger class for Milo projects.

# Usage # {#usage}
MLog is intended to be used as a part of a main project. It should be enough
to add:

~~~
include(mlog.pri)
~~~

in to the project \*.pro file.

# Features # {#features}

Main features:
1. Supports writing log messages to a file
2. Seamlessly integrates with qDebug(), qCDebug(), qInfo() etc.
3. Supports Qt categorised logging
4. Maintains 2 separate log files: current one and backup of the previous log file
5. Lightweight
6. Convenient, minimalistic API
7. (optional) Sentry support

For example usage, see example-log/main.cpp file. This project is fully
docummented - please check doxygen docs or see the comments directly in the
source file(s).

# Sentry support # {#sentry}

MLog can support [Sentry](https://docs.sentry.io), through included QRaven
module. We are running a set of custom patches over QRaven library to make it
easier to use (maybe we will send them upstream in the future). See Sentry class
for more information.

## Enabling and disabling Sentry support ## {#enable-disable-sentry}

Sentry support is not enabled by default. Please follow instructions in
mlog.pri to enable it, or look it up in our examples (example-sentry-basic
and example-sentry-full).

It is enough to add this line to your .pro file:

~~~
CONFIG += sentry
~~~

Or call qmake with -config sentry to turn it on.

To disable sentry, either don't add "sentry" to config, or call qmake with
-config no-sentry.

This disables the Sentry module completely - but thanks to the way it's build 
your in-code calls to sentry() logger will still compile. *You do not need to 
guard sentry() calls with ifdefs!*.

## Removing Sentry (I want MLog only!) ## {#remove-sentry}

If you know that your project will not need Sentry at all, feel free to copy
just MLog into your project. You can remove "sentry" directory, sentry.h
and sentry.cpp. MLog itself will still compile and work.

## Callstack support in Sentry ## {#callstack-sentry}

Not all platforms supports the backtrace. Currently we have implemented backtrace
support for unix* like platforms: MacOS, Linux. In future Windows support is
planned.

### Unix* like ### {#unix-sentry}

To enable callstack support add linker flag `-rdynamic`

~~~
    QMAKE_LFLAGS += -rdynamic
~~~

# Examples # {#examples}

MLog comes with 3 example applications:
1. example-log - shows the simplest way to include and use MLog.
2. example-sentry-basic - shows the simplest way to include Sentry.
3. example-sentry-full - shows all supported usages of Sentry in MLog.

# Sentry account # {#sentry-account}

We have a test Milo Sentry server for use in examples and testing:

**Url**: `https://sentry.milosolutions.com`

**Login**: `example@dev-qt.milosolutions.com`

**Password**: `uTh7oVahGhe`

For "real" applications, please ask our server administrators to create 
a new Sentry account for you.
[Who to contact?](https://wiki.milosolutions.com/index.php/Who_to_contact%3F)

# License # {#license}

This project is licensed under the MIT License - see the LICENSE-MiloCodeDB.txt file for details
