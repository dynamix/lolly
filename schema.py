from skip import Symbol, Schematic
import os

# this assumes the following
# * a template file that contains one led that can be cloned - named "T"
# * we assume 4 pin leds

s = Schematic('template.kicad_sch')

template = s.symbol.T
vcc = s.symbol.PWR01
gnd = s.symbol.PWR02

# gen leds
leds = []
for i in range(4):
  column = []
  for j in range(128):
    led =  template.clone()
    led.setAllReferences(f'D{i*128+j+1}')
    led.move(i*20, j*20)
    column.append(led)
  leds.append(column)

# and connect them with some wires
for col in leds:
  for led1,led2 in zip(col,col[1:]):
    w = s.wire.new()
    w.start_at(led1.pin.DO)
    w.end_at(led2.pin.DI)
    w = s.wire.new()
    w.start_at(led1.pin.CO)
    w.end_at(led2.pin.CI)

for col in leds:
  for led in col:
    w = s.wire.new()
    w.start_at(led.pin.VCC)
    w.end_at(vcc)
    w = s.wire.new()
    w.start_at(led.pin.GND)
    w.end_at(gnd)


template.delete()
s.write('lolly.kicad_sch')
