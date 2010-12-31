import os
import re

env = Environment(LIBS=['v8', 'readline', 'GL', 'GLU', 'glut'], LINKFLAGS='-Wl,--no-warn-search-mismatch')
env.VariantDir('build', 'src', duplicate=0)
env.ParseConfig('mysql_config --cflags --libs')
env.ParseConfig('sdl-config --cflags --libs')
env.ParseConfig('ncurses5-config --cflags --libs')

# Pretty output
if os.environ['TERM'] == 'dumb' :
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
gearbox = env.Program('build/gearbox', [Glob('build/*.cc'),Glob('build/modules/*.cc')])
env.Default(gearbox)

# Install target
install = env.Install('/usr/bin', gearbox)
env.Alias('install', install)

# Invocation of gear2cc in case gearbox exists
if os.path.exists('build/gearbox'):
    gear2cc = env.Command('gear2cc/gear2cc.js', 'gear2cc/gear2cc.par', 'build/gearbox gear2cc/jscc.js -v -o $TARGET -p v8 -t gear2cc/driver_v8.js_ $SOURCE')
    gears = []
    for gearFile in Glob('src/modules/*.gear', strings=True):
        gearBase = re.sub('\.gear$', '', gearFile)
        gears += [env.Command([gearBase+'.cc', gearBase+'.h'], gearFile, 'build/gearbox gear2cc/gear2cc.js src/modules/ '+re.sub('\.gear$', '', os.path.basename(gearFile)))]
    env.Depends(gears, gear2cc)
    env.Depends(gearbox, gears)
