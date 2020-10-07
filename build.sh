if [ ! -d "dist" ]; then
    mkdir dist
fi

clang -g --std=c99 -pedantic-errors -o dist/compiled src/main.c -fno-caret-diagnostics -lGL -lglfw -lGLEW
