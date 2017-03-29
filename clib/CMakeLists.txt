cmake_minimum_required(VERSION 3.4.1)

include_directories(SYSTEM workaround)
include_directories(proxmark3/client proxmark3/include proxmark3/common proxmark3/liblua)

add_definitions(-DLUA_COMPAT_ALL -DLUA_USE_LINUX -DWITH_EMV -DANDROID=1 -D__ANDROID__=1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wundef -Wformat=2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -Wchar-subscripts -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wcast-align")
# FIXME proxmark3 code quality
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-incompatible-pointer-types-discards-qualifiers")

set(UPSTREAM_SOURCES
    proxmark3/liblua/lapi.c
    proxmark3/liblua/lcode.c
    proxmark3/liblua/lctype.c
    proxmark3/liblua/ldebug.c
    proxmark3/liblua/ldo.c
    proxmark3/liblua/ldump.c
    proxmark3/liblua/lfunc.c
    proxmark3/liblua/lgc.c
    proxmark3/liblua/llex.c
    proxmark3/liblua/lmem.c
    proxmark3/liblua/lobject.c
    proxmark3/liblua/lopcodes.c
    proxmark3/liblua/lparser.c
    proxmark3/liblua/lstate.c
    proxmark3/liblua/lstring.c
    proxmark3/liblua/ltable.c
    proxmark3/liblua/ltm.c
    proxmark3/liblua/lundump.c
    proxmark3/liblua/lvm.c
    proxmark3/liblua/lzio.c
    proxmark3/liblua/lauxlib.c
    proxmark3/liblua/lbaselib.c
    proxmark3/liblua/lbitlib.c
    proxmark3/liblua/lcorolib.c
    proxmark3/liblua/ldblib.c
    proxmark3/liblua/liolib.c
    proxmark3/liblua/lmathlib.c
    proxmark3/liblua/loslib.c
    proxmark3/liblua/lstrlib.c
    proxmark3/liblua/ltablib.c
    proxmark3/liblua/loadlib.c
    proxmark3/liblua/linit.c
    proxmark3/client/aes.c
    proxmark3/client/cmdanalyse.c
    proxmark3/client/cmdcrc.c
    proxmark3/client/cmddata.c
    proxmark3/client/cmdhf.c
    proxmark3/client/cmdhf14a.c
    proxmark3/client/cmdhf14b.c
    proxmark3/client/cmdhf15.c
    proxmark3/client/cmdhfemv.c
    proxmark3/client/cmdhfepa.c
    proxmark3/client/cmdhficlass.c
    proxmark3/client/cmdhflegic.c
    proxmark3/client/cmdhfmf.c
    proxmark3/client/cmdhfmfdes.c
    proxmark3/client/cmdhfmfhard.c
    proxmark3/client/cmdhfmfu.c
    proxmark3/client/cmdhftopaz.c
    proxmark3/client/cmdhw.c
    proxmark3/client/cmdlf.c
    proxmark3/client/cmdlfawid.c
    proxmark3/client/cmdlfcotag.c
    proxmark3/client/cmdlfem4x.c
    proxmark3/client/cmdlffdx.c
    proxmark3/client/cmdlfguard.c
    proxmark3/client/cmdlfhid.c
    proxmark3/client/cmdlfhitag.c
    proxmark3/client/cmdlfio.c
    proxmark3/client/cmdlfjablotron.c
    proxmark3/client/cmdlfnedap.c
    proxmark3/client/cmdlfnoralsy.c
    proxmark3/client/cmdlfpcf7931.c
    proxmark3/client/cmdlfpresco.c
    proxmark3/client/cmdlfpyramid.c
    proxmark3/client/cmdlft55xx.c
    proxmark3/client/cmdlfti.c
    proxmark3/client/cmdlfviking.c
    proxmark3/client/cmdlfvisa2000.c
    proxmark3/client/cmdmain.c
    proxmark3/client/cmdparser.c
    proxmark3/client/cmdscript.c
    proxmark3/client/data.c
    proxmark3/client/graph.c
    proxmark3/client/loclass/cipher.c
    proxmark3/client/loclass/cipherutils.c
    proxmark3/client/loclass/des.c
    proxmark3/client/loclass/elite_crack.c
    proxmark3/client/loclass/fileutils.c
    proxmark3/client/loclass/ikeys.c
    proxmark3/client/mifarehost.c
    proxmark3/client/nonce2key/crapto1.c
    proxmark3/client/nonce2key/crypto1.c
    proxmark3/client/nonce2key/crypto1_bs.c
    proxmark3/client/nonce2key/nonce2key.c
    proxmark3/client/pm3_binlib.c
    proxmark3/client/pm3_bitlib.c
    proxmark3/client/reveng/bmpbit.c
    proxmark3/client/reveng/cli.c
    proxmark3/client/reveng/getopt.c
    proxmark3/client/reveng/model.c
    proxmark3/client/reveng/poly.c
    proxmark3/client/reveng/preset.c
    proxmark3/client/reveng/reveng.c
    proxmark3/client/scripting.c
    proxmark3/client/sleep.c
    proxmark3/client/uart.c
    proxmark3/client/util.c
    proxmark3/common/bucketsort.c
    proxmark3/common/crc.c
    proxmark3/common/crc16.c
    proxmark3/common/crc64.c
    proxmark3/common/iso14443crc.c
    proxmark3/common/iso15693tools.c
    proxmark3/common/legic_prng.c
    proxmark3/common/lfdemod.c
    proxmark3/common/parity.c
    proxmark3/common/prng.c
    proxmark3/common/protocols.c
    proxmark3/common/sha1.c
    proxmark3/common/sha256.c
    proxmark3/common/tea.c
    )

add_library(proxmark3 SHARED ${UPSTREAM_SOURCES} glue.c)
target_link_libraries(proxmark3 m atomic)