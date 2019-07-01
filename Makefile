all : xs xr

xs : xs.c
	cc -o xs xs.c

xr : xr.c
	cc -o xr xr.c

