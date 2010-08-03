#!/usr/bin
make distclean
make clean
make sq32c8006b
make all
#cp EVBV1_SQ8000_ARM9_v03.00.bin /mnt/hgfs/share/tftpboot/sq8000_dia.bin
#cp EVBV1_SQ8000_ARM9_v03.00.bin /tftpboot/sq8000_dia.bin
cp PDK_PC9223_ARM9_v09.01.bin /mnt/hgfs/share/tftpboot/9223.bin
cp PDK_PC9223_ARM9_v09.01.bin /tftpboot/9223.bin