; turbo_grind.nc  — continuous Y pull at Turbo speed

(O01_LOOP)
  M66 P1 L4 Q0 S1    ; if turbo_btn is LOW (pressed), skip next line
  M66 P1 L3 Q0 S0    ; wait here until button goes HIGH (released)
  G90
  G91
  G0 Y-2 F1000       ; pull 2 mm at 1000 mm/min
  G90
  G4 P50
  GOTO O01_LOOP
