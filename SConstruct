import os
import re
import sys

env = Environment()

crossmingw = ARGUMENTS.get('crossmingw', 0)
env['USING_CROSSMINGW'] = False
if int(crossmingw):
    env.Tool('crossmingw', toolpath = ['#tools'])
    env['USING_CROSSMINGW'] = True

# Windows gets special treatement
if sys.platform == 'win32' or env['USING_CROSSMINGW']:
    if not os.path.exists('contrib'):
        print ""
        print "============================================================================="
        print "For building v8-gearbox on windows you need to make a directory named contrib"
        print "and put the following in there: v8 (you need to build it yourself), readline,"
        print "freeglut, pdcurses, mysql-connector-c and SDL. Also you can only use MinGW to"
        print "build v8-gearbox. Have a nice day :)"
        print "============================================================================="
        print ""
        sys.exit(1)
    env.Append(CPPPATH = Glob(os.path.join('contrib', '*', 'include'), strings=True))
    env.Append(CPPPATH = Glob(os.path.join('contrib', '*'), strings=True))
    env.Append(CPPPATH = 'src')
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*', 'bin'), strings=True))
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*', 'lib'), strings=True))
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*'), strings=True))
    env.Append(LINKFLAGS = '-static-libgcc -static-libstdc++')
    env.Append(CXXFLAGS = '-std=c++0x -O3 -fno-var-tracking-assignments')
    env.Append(LIBS = ['v8', 'readline', 'opengl32', 'glu32', 'freeglut', 'curses', 'pthread', 'mysql', 'ws2_32' , 'winmm', 'SDL', 'SDLmain'])
else:
    env.ParseConfig('mysql_config --cflags --libs')
    env.Append(CPPPATH = '#src')
    env.Append(LINKFLAGS = '-Wl,--no-warn-search-mismatch')
    env.Append(CXXFLAGS = '-std=c++0x -O3 -fno-var-tracking-assignments')
    env.Append(LIBS = ['v8', 'readline', 'GL', 'GLU', 'glut', 'SDL'])

# Pretty output
if sys.platform == 'win32' or not hasattr(os.environ, 'TERM') or os.environ['TERM'] == 'dumb':
    env['CCCOMSTR']   =    '     Compiling $SOURCES -> $TARGET'
    env['CXXCOMSTR']  =    '     Compiling $SOURCES -> $TARGET'
    env['ASCOMSTR']   =    '    Assembling $SOURCES -> $TARGET'
    env['LINKCOMSTR'] =    '       Linking $SOURCES -> $TARGET'
    env['ARCOMSTR']   =    '     Archiving $SOURCES -> $TARGET'
    env['RANLIBCOMSTR'] =  '      Indexing $SOURCES -> $TARGET'
    env['NMCOMSTR']   =    '  Creating map $SOURCES -> $TARGET'
    env['DOCCOMSTR']  =    '   Documenting $SOURCES -> $TARGET'
    env['TARCOMSTR']  =    '      Creating $SOURCES -> $TARGET'
    env['JSCCCOMSTR']  =   '     Compiling $SOURCES -> $TARGET'
    env['GEAR2CCCOMSTR']  ='    Converting $SOURCES -> $TARGETS'
else:
    env['CCCOMSTR']   =    '     Compiling \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['CXXCOMSTR']  =    '     Compiling \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['ASCOMSTR']   =    '    Assembling \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['LINKCOMSTR'] =    '       Linking \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['ARCOMSTR']   =    '     Archiving \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['RANLIBCOMSTR'] =  '      Indexing \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['NMCOMSTR']   =    '  Creating map \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['DOCCOMSTR']  =    '   Documenting \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['TARCOMSTR']  =    '      Creating \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['JSCCCOMSTR']   =  '     Compiling \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'
    env['GEAR2CCCOMSTR']  ='    Converting \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGETS\033[0m'
    
env['BUILD_DIR'] = 'build'
env['GEARBOX'] = env['BUILD_DIR'] + os.sep + 'gearbox' + env['PROGSUFFIX']
env['GEARBOX_EXISTS'] = os.path.exists(env['GEARBOX'])
env['BUILDERS']['JsCC'] = Builder(action=Action(env['GEARBOX']+' gear2cc'+os.sep+'jscc.js -o $TARGET -p v8 -t gear2cc'+os.sep+'driver_v8.js_ $SOURCE', cmdstr=env['JSCCCOMSTR']))
env['BUILDERS']['Gear2CC'] = Builder(action=Action(env['GEARBOX']+' gear2cc'+os.sep+'gear2cc.js $SOURCE', cmdstr=env['GEAR2CCCOMSTR']))

SConscript('src/SConscript', variant_dir = env['BUILD_DIR'], exports = ['env'], duplicate = 0)
