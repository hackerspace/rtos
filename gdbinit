set history expansion on
set history filename .gdb_history
set history save
layout split
fs cmd
wh cmd 12
target remote:1234
load
