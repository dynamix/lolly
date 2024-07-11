import pcbnew
import math


board = pcbnew.GetBoard()

ranges = [
  [497, 507],
  [467, 478],
  [452, 463],
  [416, 425],
  [408, 415],
  [367, 371],
  [363, 366],
]

rotate = [318]

for r in ranges:
  for i in range(r[0], r[1]+1):
    rotate.append(i)

def vector(length, angle, scale = SCALE):
  return pcbnew.VECTOR2I(int(length * math.sin(angle) * scale), int(length * math.cos(angle) * scale))

def draw_via(pos, layer, net, diameter = 0.8 * SCALE, drill = 0.4 * SCALE):
  via = pcbnew.PCB_VIA(board)
  via.SetPosition(pos)
  via.SetLayer(layer)
  via.SetWidth(int(diameter))
  via.SetDrill(int(drill))
  board.Add(via)
  via.SetNet(net)
  return via


for led_idx in rotate:
  led = board.FindFootprintByReference("D{}".format(led_idx))
  

  din_pad, cin_pad, gnd_pad, vcc_pad, cout_pad, dout_pad  = led.Pads()

  rotate = led.GetOrientationDegrees()
  rotate_rad = (rotate / 180.) * math.pi
  
  offset= vector(2, rotate_rad - math.pi / 2)
  # via = draw_via(gnd_pad.GetCenter() + offset, pcbnew.F_Cu, gnd_pad.GetNet())
  draw_track(gnd_pad.GetCenter(), gnd_pad.GetCenter() + offset, pcbnew.F_Cu, gnd_pad.GetNet(), 0.8 * SCALE)

  

  # led.SetOrientationDegrees(led.GetOrientationDegrees() + 180)
  
pcbnew.Refresh()

