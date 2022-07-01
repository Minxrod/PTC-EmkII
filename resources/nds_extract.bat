:: Usage: nds_extract.bat <ptc_file_name>.nds

set NDS_FILE=%1
:: sdatxtract extraction path
set TEMP_PATH=%NDS_FILE:~0,-4%\0\
:: ndstool extraction path
set EXTRACT_PATH=extract\

@echo Found .nds file: %NDS_FILE%

@echo Creating directories...
mkdir "ui"
mkdir "graphics"
mkdir "sounds"

@echo Extracting graphics resources...
tools\ndstool.exe -x -d %EXTRACT_PATH% %NDS_FILE%
@echo Extracting sound resources...
tools\sdatxtract.exe %NDS_FILE%

@echo Moving graphics resources...
move %EXTRACT_PATH%*.NSCR ui\
move %EXTRACT_PATH%*.NCGR ui\
move %EXTRACT_PATH%*.NCLR ui\

echo "Moving sample programs..."
for /F %f in (%EXTRACT_PATH%*.PRG) do (
	python tools/txt_to_ptc.py %f
)
move *.PTC ..\programs\

@echo Extracting NCGR to .PTC
python "tools/ncgr_to_ptc.py" "ui/BGU.NCGR" "BGU"
python "tools/ncgr_to_ptc.py" "ui/BGD.NCGR" "BGD"
python "tools/ncgr_to_ptc.py" "ui/BGF.NCGR" "BGF"
python "tools/ncgr_to_ptc.py" "ui/partsSPU.NCGR" "SPU"
python "tools/ncgr_to_ptc.py" "ui/partsSPS.NCGR" "SPS"
python "tools/ncgr_to_ptc.py" "ui/partsSPDK.NCGR" "SPD"

@echo Extracting NCLR to .PTC
python "tools/nclr_to_ptc.py" "ui/defBGSP.NCLR" "COL0"
python "tools/nclr_to_ptc.py" "ui/defBGSP.NCLR" "COL1"
python "tools/nclr_to_ptc.py" "ui/GRP.NCLR" "COL2"

@echo Moving extracted PTC...
move BGU*.PTC graphics\
move BGD*.PTC graphics\
move BGF*.PTC graphics\
move SPU*.PTC graphics\
move SPS*.PTC graphics\
move SPD*.PTC graphics\
move COL*.PTC graphics\

@echo Moving sound resources...
move %TEMP_PATH%bank\*.sbnk sounds\
move %TEMP_PATH%sequence\*.sseq sounds\
move %TEMP_PATH%wave\*.swar sounds\

@echo Removing extraction directories
del %TEMP_PATH%
rmdir %TEMP_PATH%
del %EXTRACT_PATH%
rmdir %EXTRACT_PATH%
