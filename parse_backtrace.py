import os
import sys

binary = r'C:\Users\ytzur\.platformio\packages\toolchain-xtensa-esp32\bin\xtensa-esp32-elf-addr2line.exe'
elf = r'C:\workspace\Projects\Friends\Noked\.pio\build\upesy_wroom\firmware.elf'

def main():
  if len(sys.argv) < 2:
    print(f'USAGE: python3 {sys.argv[0]} <backtrace>')
    return

  backtrace = sys.argv[1]
  for trace in backtrace.split(' '):
    os.system(f'{binary} -e {elf} -a {trace}')

if __name__ == '__main__':
  main()

