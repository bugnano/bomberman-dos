link_options =
#link_options = debug all
compiler = wcc
compile_options = /onatx /oh /ei /zp8 /0 /fpi87 /q /d1
#compile_options = /d2 /0 /q
header_files = video.h timer.h keyboard.h
object_files = main.obj video.obj timer.obj keyboard.obj
program_dir = src\

.c: $(program_dir)

.c.obj: $(header_files)
	$(compiler) $(compile_options) $[*

out\game.exe : $(object_files)
	wlink $(link_options) name $@ file { $(object_files) }

