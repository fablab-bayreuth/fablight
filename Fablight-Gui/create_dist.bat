
pyinstaller  --clean  --distpath=bin\ -i fablight.ico  fablight_gui.py
ren  bin\fablight_gui  fablight_gui_win
copy  fablight.gif  bin\fablight_gui_win

del *.spec
rd /s /q  build
rd /s /q  bin\fablight_gui_win\include
rd /s /q  bin\fablight_gui_win\_MEI\tcl\encoding
rd /s /q  bin\fablight_gui_win\_MEI\tcl\http1.0
rd /s /q  bin\fablight_gui_win\_MEI\tcl\msgs
rd /s /q  bin\fablight_gui_win\_MEI\tcl\opt0.4
rd /s /q  bin\fablight_gui_win\_MEI\tcl\tzdata
rd /s /q  bin\fablight_gui_win\_MEI\tk\images
rd /s /q  bin\fablight_gui_win\_MEI\tk\msgs
del /s /q  bin\fablight_gui_win\numpy.core._dotblas.pyd

cd bin
zip -r fablight_gui_win.zip  fablight_gui_win\*
cd ..

