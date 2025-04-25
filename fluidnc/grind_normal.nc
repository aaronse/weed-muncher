; normal_grind.nc  — continuous Y pull at Normal speed

(O00_LOOP)
  M66 P0 L4 Q0 S1    ; if normal_btn is LOW (pressed), skip next line
  M66 P0 L3 Q0 S0    ; wait here until button goes HIGH (released)
  G90                ; absolute coords
  G91                ; then switch to relative
  G0 Y-2 F500        ; pull 2 mm at 500 mm/min
  G90                ; back to absolute
  G4 P50             ; tiny dwell (50 ms)
  GOTO O00_LOOP      ; loop as long as button remains down
