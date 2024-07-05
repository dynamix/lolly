import pcbnew
import math

LEDS = 512
LED_SPACING = 10.5
SCALE = 1e6

board = pcbnew.GetBoard()

x = 300 * SCALE
y = 300 * SCALE


LED_MAP = []
LED_ROTATE = []


def draw_via(pos, layer, net, diameter = 0.8 * SCALE, drill = 0.4 * SCALE):
  via = pcbnew.PCB_VIA(board)
  via.SetPosition(pos)
  via.SetLayer(layer)
  via.SetWidth(int(diameter))
  via.SetDrill(int(drill))
  board.Add(via)
  via.SetNet(net)
  return via


def draw_track(start, end, layer, net, width = None):
  track = pcbnew.PCB_TRACK(board)
  track.SetStart(start)
  track.SetEnd(end)
  track.SetLayer(layer)
  board.Add(track)
  track.SetNet(net)
  if width:
    track.SetWidth(int(width))
  return track


def vector(length, angle, scale = SCALE):
  return pcbnew.VECTOR2I(int(length * math.sin(angle) * scale), int(length * math.cos(angle) * scale))


prev_dout = None

disconnect = [1,50,46,6,496,478,451,425,407,371,362,317,195,150,140,106,87,61,34,18,507,466,463,295,288,250,235,206,182,162,129,118,73,75,440,437,395,383,350,330,307,277,262,223,218, 318, 194]

# q4
disconnect += [487, 47, 48, 49, 2,3,4,5,6, 268, 220]

# q3
disconnect += [342, 391, 438, 390, 341, 171, 122]

# q2
disconnect += [25,292,196, 244]

# q1
disconnect += [366,363,146,97,147, 415]


# reverse q4
reverse = [2,3,4,5,6, 49]

# reverse q3
reverse += [439, 392, 342]

# reverse q2
reverse += [293,245, 197]

# reverse q1
reverse += [363, 318]


# new manual connections
new_conns = [ 
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
  #q3
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
  [342, 343],
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


for stripidx, j in enumerate([1, 9, 17, 4, 12, 20, 7, 15, 2, 10, 18, 5, 13, 21, 8, 16, 3, 11, 19, 6, 14]):
  strip = []
  for i in range(j, LEDS + 1, 21):
    strip.append(i)
    LED_ROTATE.append(180 if stripidx % 2 == 0 else 0)
  if stripidx % 2 == 1:
    strip.reverse()
  LED_MAP += strip

for led_idx in range(1, LEDS + 1):
  n = LED_MAP[led_idx - 1]
  r = LED_SPACING * math.sqrt(n)
  deg = n * 137.508
  angle = (deg / 180) * math.pi
  pos = pcbnew.VECTOR2I(int(r * math.cos(angle) * SCALE + x), int(r * math.sin(angle) * SCALE + y))
  led = board.FindFootprintByReference("D{}".format(led_idx))
  led.SetPosition(pos)

  rotate = LED_ROTATE[led_idx - 1] - deg + 90
  
  # manually correct some leds on the boarder
  if led_idx in reverse:
    rotate += 180
  
  led.SetOrientationDegrees(rotate)
  

  din_pad, cin_pad, gnd_pad, vcc_pad, cout_pad, dout_pad  = led.Pads()

  if led_idx-1 in disconnect:
    prev_dout = None
  if prev_dout:
    draw_track(prev_dout.GetCenter(), din_pad.GetCenter(), pcbnew.F_Cu, din_pad.GetNet(), 0.25 * SCALE)
    draw_track(prev_cout.GetCenter(), cin_pad.GetCenter(), pcbnew.F_Cu, cin_pad.GetNet(), 0.25 * SCALE)
  
  prev_dout, prev_cout = dout_pad, cout_pad

  rotate_rad = (rotate / 180.) * math.pi
  # offset= pcbnew.VECTOR2I(int(5*SCALE),int(5*SCALE))
  offset= vector(2, rotate_rad - math.pi / 2)
  via = draw_via(gnd_pad.GetCenter() + offset, pcbnew.F_Cu, gnd_pad.GetNet())

  draw_track(gnd_pad.GetCenter(), via.GetCenter(), pcbnew.F_Cu, gnd_pad.GetNet(), 0.8 * SCALE)

for conn in new_conns:
  led1 = board.FindFootprintByReference("D{}".format(conn[0]))
  led2 = board.FindFootprintByReference("D{}".format(conn[1]))
  
  dout = led1.Pads()[5]
  din = led2.Pads()[0]
  cout = led1.Pads()[4]
  cin = led2.Pads()[1]
  draw_track(dout.GetCenter(), din.GetCenter(), pcbnew.F_Cu, dout.GetNet(), 0.25 * SCALE)
  draw_track(cout.GetCenter(), cin.GetCenter(), pcbnew.F_Cu, cout.GetNet(), 0.25 * SCALE)

  # draw_track(led1.GetPosition(), led2.GetPosition(), pcbnew.F_Cu, led1.GetNet(), 0.25 * SCALE)


pcbnew.Refresh()

  # 
  # draw_track(via.GetCenter(), cap_gnd.GetCenter(), pcbnew.F_Cu, via.GetNet(), 0.4 * SCALE)
