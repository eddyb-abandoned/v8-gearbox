import os
import re
import sys

env = Environment()
env.VariantDir('build', 'src', duplicate=0)

# Windows gets special treatement
if sys.platform == 'win32':
    if not os.path.exists('contrib'):
        print ""
        print "============================================================================="
        print "For building v8-gearbox on windows you need to make a directory named contrib"
        print "and put the following in there: v8 (you need to build it yourself), readline,"
        print "freeglut, pdcurses, mysql-connector-c and SDL. Also, you can only use VC++ to"
        print "build v8-gearbox. Have a nice day :)"
        print "============================================================================="
        print ""
        sys.exit(1)
    env.Append(CPPPATH = Glob(os.path.join('contrib', '*', 'include'), strings=True))
    env.Append(CPPPATH = Glob(os.path.join('contrib', '*'), strings=True))
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*', 'bin'), strings=True))
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*', 'lib'), strings=True))
    env.Append(LIBPATH = Glob(os.path.join('contrib', '*'), strings=True))
    env.Append(CXXFLAGS = '-std=c++0x')
    env.Append(LIBS = ['v8', 'readline', 'OpenGL32', 'GLU32', 'freeglut', 'pdcurses', 'libmysql', 'ws2_32' , 'winmm', 'SDL', 'SDLmain'])
else:
    env.ParseConfig('mysql_config --cflags --libs')
    env.ParseConfig('sdl-config --cflags --libs')
    env.ParseConfig('ncurses5-config --cflags --libs')
    env.Append(LINKFLAGS = '-Wl,--no-warn-search-mismatch')
    env.Append(CXXFLAGS = '-std=c++0x -O3 -fno-var-tracking-assignments')
    env.Append(LIBS = ['v8', 'readline', 'GL', 'GLU', 'glut'])

# Pretty output
if sys.platform == 'win32' or os.environ['TERM'] == 'dumb':
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
    env['GEAR2CCCOMSTR']  ='    Converting $SOURCES -> $TARGET'
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
    env['GEAR2CCCOMSTR']  ='    Converting \033[32m$SOURCES\033[0m\033[1m -> \033[0m\033[32m$TARGET\033[0m'

# Gearbox target
gearboxPath = os.path.join('build', 'gearbox')
gearboxSources = [
    'build/shell.cc',
    'build/Gearbox.cc',
    'build/global.cc',
    'build/modules/GL.cc',
    'build/modules/Io.cc',
    'build/modules/MySQL.cc',
    'build/modules/Ncurses.cc',
    'build/modules/Network.cc',
    'build/modules/SDL.cc',
]
gearbox = env.Program(gearboxPath, gearboxSources)
env.Default(gearbox)
env.Precious(gearbox)

# Install target (not on windows)
if sys.platform != 'win32':
    install = env.Install('/usr/bin', gearbox)
    env.Alias('install', install)

# Invocation of gear2cc in case gearbox exists
gearboxPath += env['PROGSUFFIX']
gears = []
if os.path.exists(gearboxPath):
    gear2ccPath = 'gear2cc' + os.sep
    modulePath = os.path.join('src','modules') + os.sep
    
    # JsCC builder
    env['BUILDERS']['JsCC'] = Builder(action=Action(gearboxPath+' '+gear2ccPath+'jscc.js -o $TARGET -p v8 -t '+gear2ccPath+'driver_v8.js_ $SOURCE', cmdstr=env['JSCCCOMSTR']))
    gear2cc = env.JsCC(gear2ccPath+'gear2cc.js', gear2ccPath+'gear2cc.par')
    
    # Gear2CC builder
    def gear2cc_action(target, source, env):
        Execute(gearboxPath+' '+gear2ccPath+'gear2cc.js '+modulePath+' '+re.sub('\.gear$','',os.path.basename(source[0].rstr())), cmdstr=None)
    env['BUILDERS']['Gear2CC'] = Builder(action=Action(gear2cc_action, cmdstr=env['GEAR2CCCOMSTR']))

    # Build all gear files
    for gearFile in Glob(os.path.join('src', 'modules', '*.gear'), strings=True):
        gearBase = re.sub('\.gear$', '', gearFile)
        gear = env.Gear2CC([gearBase+'.cc', gearBase+'.h'], gearFile)
        env.Depends(gear, gear2cc)
        env.Depends(gearbox, gear)

