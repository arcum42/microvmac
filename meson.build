# Project definition
project('microvmac', ['c', 'cpp'], default_options: ['warning_level=3', 'prefer_static=true', 'c_std=c2x', 'cpp_std=c++20'])

# Configuration data
conf = configuration_data({
	# verify ROM checksum on startup?
	'RomStartCheckSum': 1,
	# Enable drag/drop of disk images?
	'EnableDragDrop': 1,
	# Allow user to choose location for new disks?
	'SaveDialogEnable': 1,
	# Start in full-screen mode?
	'WantInitFullScreen': 0,
	# Full screen is an option?
	'MayFullScreen': 1,
	# Windowed is an option?
	'MayNotFullScreen': 1,
	# Start magnified?
	'WantInitMagnify': 0,
	# Magnification power
	'WindowScale': 2,
	# Allow background running by default?
	'WantInitRunInBackground': 1,
	# Initial clock multiplied (0 = 1x)
	'WantInitSpeedValue': 0,
	# Enable interrupt key? (on prog. switch)
	'WantEnblCtrlInt': 1,
	# Enable reset key? (on prog. switch)
	'WantEnblCtrlRst': 1,
	# Emulated control key toggle via Ctrl+K?
	'WantEnblCrtlKtg': 1,
	# Use Ctrl+num to insert "./disk#.dsk"
	'NeedRequestIthDisk': 0,
	# Disk open dialog on Ctrl+O
	'NeedRequestInsertDisk': 1,
	# Special > More Commands tutorial message
	'NeedDoMoreCommandsMsg': 1,
	# Enable about dialog
	'NeedDoAboutMsg': 1,
	# Include international characters for Control Mode
	'NeedIntlChars': 0,
	# Force keyboard to match Mac layout on Windows
	'ItnlKyBdFix': 1,
	# Version information
	'kStrAppName': '"micro vMac"',
	'kAppVariationStr': '"0.37.0"',
	'kStrCopyrightYear': '"1996-2023"',
	'kMaintainerName': '"InvisibleUp"',
	'kStrHomePage': '"https://github.com/invisibleup/uvmac/"',
	'kBldOpts': '"obsolete"'
})
configure_file(
	output: 'CNFGRAPI.h',
	configuration: conf
)

# Dependencies
lSDL2 = dependency('SDL2')
lspdlog = dependency('spdlog')
ljson = dependency('nlohmann_json')

# Windows resources
if target_machine.system() == 'windows'
	windows = import('windows')
	WIN_RSRC = windows.compile_resources(
		'rsrc/WIN32/main.rc',
		depend_files: [
			'rsrc/WIN32/app.manifest',
			'rsrc/WIN32/ICONAPPW.ico',
			'rsrc/WIN32/ICONDSKW.ico',
			'rsrc/WIN32/ICONROMW.ico',
			'rsrc/SONYDRV.bin',
			'rsrc/SONYICO.bin',
		]
	)
else
	WIN_RSRC = []
endif

# Hardware libraries
HW_SRC = {
	'HW': [
		'src/HW/hardware.cpp',
		'src/HW/wires.cpp'
	],
	'ADB':  [
		'src/HW/ADB/adb.cpp'
	],
	'DISK': [
		'src/HW/DISK/int_woz_machine.cpp',
		'src/HW/DISK/sony_floppy.cpp',
		'src/PATCHES/sony_driver_hack.cpp'
    ],
    'KBRD': [
		'src/HW/KBRD/keyboard.cpp',
	],
	'M68K': [
#		'src/HW/M68K/disassembly_68k.cpp',
		'src/HW/M68K/instruct_table_68k.cpp',
		'src/HW/M68K/cpu_68k.cpp',
#		'src/HW/M68K/fpu.cpp',
	],
	'MOUSE': [
		'src/HW/MOUSE/mouse.cpp',
	],
	'POWERMAN': [
		'src/HW/POWERMAN/power.cpp',
	],
	'RTC': [
		'src/HW/RTC/rtc.cpp',
	],
	'SCC': [
		'src/HW/SCC/serial_comm.cpp',
	],
	'SCREEN': [
		'src/HW/SCREEN/screen.cpp',
		'src/PATCHES/screen_hack.cpp',
	],
	'SCSI': [
		'src/HW/SCSI/scsi.cpp',
	],
	'SOUND': [
		'src/HW/SOUND/sound.cpp',
	],
	'SOUND_ASC': [
		'src/HW/SOUND/apple_sound_chip.cpp',
	],
	'VIA': [
		'src/HW/VIA/via.cpp',
	],
	'VIA1': [
		'src/HW/VIA/via1.cpp',
	],
	'VIA2': [
		'src/HW/VIA/via2.cpp',
	],
	'VIDCARD': [
		'src/HW/VIDCARD/video.cpp',
	],
	'RAM': [
		'src/HW/RAM/ram.cpp',
	],
	'ROM': [
		'src/HW/ROM/rom.cpp',
	]
}

MAC_SRC_COMMON = \
		HW_SRC['DISK'] + \
		HW_SRC['M68K'] + \
		HW_SRC['RTC'] + \
		HW_SRC['SOUND'] + \
		HW_SRC['VIA'] + \
		HW_SRC['VIA1'] + \
		HW_SRC['SCREEN'] + \
		HW_SRC['SCC'] + \
		HW_SRC['SCSI'] + \
		HW_SRC['MOUSE'] + \
		HW_SRC['RAM'] + \
		HW_SRC['ROM'] + \
		HW_SRC['HW']

# Macintosh definitions
MAC_SRC = {
#	'Plus': 
#		MAC_SRC_COMMON +
#		HW_SRC['KBRD'],
#	'II': 
#		MAC_SRC_COMMON +
#		HW_SRC['VIDCARD'] +
#		HW_SRC['ADB'] +
#		HW_SRC['VIA2'] +
#		HW_SRC['SOUND_ASC'],
	'All':
		MAC_SRC_COMMON +
		HW_SRC['KBRD'] +
		HW_SRC['VIDCARD'] +
		HW_SRC['ADB'] +
		HW_SRC['VIA2'] +
		HW_SRC['SOUND_ASC']
}

# User interface definitions
UI_SRC = [
	'src/UI/event_queue.cpp',
	'src/UI/memory.cpp',
	'src/UI/os_glue.cpp',
	'src/UI/clipboard_sdl2.cpp',
	'src/UI/drives_sdl2.cpp',
	'src/UI/intl_sdl2.cpp',
	'src/UI/keyboard_sdl2.cpp',
	'src/UI/mouse_sdl2.cpp',
	'src/UI/rom_sdl2.cpp',
	'src/UI/sound_sdl2.cpp',
	'src/UI/time_date_sdl2.cpp',
	'src/UI/video_sdl2.cpp',
]

EMU_SRC = [
	'src/mac_main.cpp',
	'src/global_glue.cpp',
	'src/ticks.cpp',
	'src/config.cpp',
	'src/PATCHES/rom.cpp',
	'src/UTIL/date_to_seconds.cpp',
	'src/UTIL/dbglog.cpp',
	'src/UTIL/param_buffers.cpp',
	'src/LANG/intl_chars.cpp',
]

IMGUI_SRC = [
	'lib/imgui/imgui_draw.cpp',
	'lib/imgui/imgui_tables.cpp',
	'lib/imgui/imgui_widgets.cpp',
	'lib/imgui/imgui.cpp',
	'lib/imgui/backends/imgui_impl_sdl2.cpp',
	'lib/imgui/backends/imgui_impl_sdlrenderer2.cpp',
	'lib/imgui/imgui_demo.cpp'

]
EMU_INC = include_directories([
	'cfg/',
	'src/',
	'lib/',
	'lib/imgui/',
	'lib/imgui/backends'
])

## Final compiled program definition
# Just gonna do an SDL2 Mac Plus for now
executable(
	'microvmac',
	sources: MAC_SRC['All'] + UI_SRC + EMU_SRC + IMGUI_SRC + WIN_RSRC,
	dependencies: [lSDL2, lspdlog, ljson],
	include_directories: EMU_INC,
	win_subsystem: 'windows'
)

