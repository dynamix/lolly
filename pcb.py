import pcbnew
import math

LEDS = 512
LED_SPACING = 10.5
SCALE = 1e6

board = pcbnew.GetBoard()

x = 300 * SCALE
y = 300 * SCALE


for led_idx in range(1, LEDS + 1):
  n = led_idx
  r = LED_SPACING * math.sqrt(n)
  theta_deg = n * 137.508
  theta = (theta_deg / 180) * math.pi
  pos = pcbnew.VECTOR2I(int(r * math.cos(theta) * SCALE + x), int(r * math.sin(theta) * SCALE + y))
  mod = board.FindFootprintByReference("D{}".format(led_idx))
  mod.SetPosition(pos)
