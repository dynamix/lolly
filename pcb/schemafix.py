from skip import Symbol, Schematic
import os

# this assumes the following
# * a template file that contains one led that can be cloned - named "T"
# * we assume 4 pin leds

s = Schematic('lolly.kicad_sch')


def connect(led1,led2):
  w = s.wire.new()
  w.start_at(led1.pin.DO)
  w.end_at(led2.pin.DI)
  w = s.wire.new()
  w.start_at(led1.pin.CO)
  w.end_at(led2.pin.CI)

def disconnect(led):
  for pin in ['DI', 'CI','DO', 'CO']:
    for w in led.pin[pin].attached_wires:
      # print(f"Disconnecting {pin} from {w}")
      w.delete()


dis = [
  [18, 497],
  [452, 478],
  [507, 467],
  [416, 463],
  [408, 425],
  [367, 415],
]

# for leds in dis:
#   disconnect(s.symbol[f"D{leds[0]}"])
#   disconnect(s.symbol[f"D{leds[1]}"])




ranges = [
  # [507, 497],
  # [478, 467],
  # [ 463, 452],
  # [425, 416 ],
  # [415, 408],
  # [371, 367],
  [366, 363],
]


prev = None
for r in ranges:
  a = r[0]
  b = r[1]
  step = 1
  if a > b:
    step = -1
  prev = None
  for i in range(a, b+step, step):
    if prev:
      # print(f"conn {prev}-{i}")
      pass
      # disconnect(s.symbol[f"D{prev}"])
      # disconnect(s.symbol[f"D{i}"])

      # connect(s.symbol[f"D{prev}"], s.symbol[f"D{i}"])
    prev = i


# disconnect(s.symbol[f"D318"])


conn = [ 
  [17,18],
  [463,462],
  [462,461],
  [461,460],
  [460,459],
  [459,458],
  [458,457],
  [457,456],
  [456,455],
  [455,454],
  [454,453],
  [453,452],
  [452,425],
  # [18, 507],
  # [467, 463],
  # [452, 425],
  # [363, 408],
  # [416, 415],
  # [367, 366],
  # [363, 318],
  # [408, 363],
  #  [497, 478],
  #  [467, 463],
    # [452, 425],
    # [408,371]

]

for leds in conn:
  # print(f"Connecting {leds[0]} to {leds[1]}")
  connect(s.symbol[f"D{leds[0]}"], s.symbol[f"D{leds[1]}"])



s.write('lolly.kicad_sch')
