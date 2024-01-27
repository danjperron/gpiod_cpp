This is s simple example to  use GPIO on raspberry Pi4 & 5 in c++.

It uses  simple functions.

- bool pinMode(int pin,int  mode);<br>
  Select what you want to do with the GPIO<br>
  OUTPUT, INPUT, INPUT_PULLUP, INPUT_PULLDOWN, OPENDRAIN_PULLUP.<br>
  return true if it works. False is the Pin was already exported somewhere else.<br>

- void digitalWrite(int pin,int  output);<br>
  write digital value to gpio.

- int digitalRead(int pin);<br>
  read digital value of the gpio.

- void release_gpiod();<br>
  When you done just release

- init_gpiod();<br>
  This initialize gpiod (get chipgpiod).<br>
  Any PinMode will automatically call init_gpiod if it wasn't.


N.B. you need libgpiod<br>
  sudo apt-get install gpiod libgpiod-dev libgpiod-doc

To compile just run make and execute ./test

