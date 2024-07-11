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
h = {}
parts = [0,244]
all = parts + [512] 

disconnect_out = [1,50,46,6,496,478,451,425,407,371,362,317,195,150,140,106,87,61,34,18,507,466,463,295,288,250,235,206,182,162,129,118,73,75,440,437,395,383,350,330,307,277,262,223,218, 318, 194]

# q4
disconnect_out += [49, 2,3,4,5,6, 268, 220]

#q3
disconnect_out += [391, 438, 341, 171, 122]

# q2
disconnect_out += [25,292,196, 244]

# q1
disconnect_out += [366,363,146,97,147, 415]

# q4
disconnect_in = [49,50,488,48,47, 2,3,4,5,6]

# q3
disconnect_in += [342, 391]

#q2
disconnect_in += []

#q2
disconnect_in += [147]

new_conns = [ 
  # q4
  [1,49],
  [49,50],
  [50,48],
  [48,47],
  [47,6],
  [2,488],
  [496, 479],
  [451, 426],
  [407, 372],
  [362, 319],
  [487,441],
  [437, 396],
  [383, 351],
  [330, 308],
  [277, 263],
  [223, 219],

  [6,5],
  [5,4],
  [4,3],
  [3,2],

  [317, 269],
  [268, 221],

  # q3
  [391, 439],
  [440, 438],
  [438, 392],
  [395, 384],
  [350, 331],
  [307, 278],
  [262, 224],
  [218, 172],
  [288, 251],
  [235, 207],
  [182, 163],
  [129, 119],
  [390, 342],
  [341, 296],
  [171,123],
  [122, 74],

  #q2
  [194, 151],
  [140, 107],
  [87, 62],
  [34, 19],
  [466, 508],
  [73, 26],
  [118, 76],
  [162, 130],
  [206, 183],
  [250, 236],
  [295, 289],
  [292, 245],
  [295, 289],
  [196, 293],
  [244, 197],
  [25,464],

   #q1
  [150, 141],
  [106, 88],
  [61, 35],
  [507, 467],
  [463, 416],
  [371, 363],
  [363, 318],
  [318, 364],
  [425, 408],
  [478, 452],
  [46, 7],
  [150, 141],
  [146, 98],
  [147, 195],
  [195, 148],
  [97, 51],
  [18, 497],

  [415, 367],

  


]


def connect(led1,led2):
  w = s.wire.new()
  w.start_at(led1.pin.DO)
  w.end_at(led2.pin.DI)
  w = s.wire.new()
  w.start_at(led1.pin.CO)
  w.end_at(led2.pin.CI)

for idx, i in enumerate(parts):
  column = []
  print(idx)
  for j in range(all[idx+1] - all[idx]):
    led =  template.clone()
    id = i+j+1
    led.setAllReferences(f'D{id}')
    led.lid = id
    led.move(idx*20, j*20)
    column.append(led)
    h[id] = led
  leds.append(column)

# and connect them with some wires
for col in leds:
  for led1,led2 in zip(col,col[1:]):

    if led1.lid in disconnect_out:
      continue

    if led2.lid in disconnect_in:
      continue

    connect(led1,led2)

for conn in new_conns:
  led1 = h[conn[0]]
  led2 = h[conn[1]]
  connect(led1,led2)

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
