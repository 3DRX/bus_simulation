mkdir ./combined
cat >> ./combined/combined.c $(find ./ -name '*.c')
cat >> ./combined/combined.h $(find ./ -name '*.h')

