mruby-minigame
==============

mruby-minigame is a 2D game framework for rapid prototyping.

example:

``` ruby
include Minigame

# must create display at first.
Display.create 640, 480

img = Image.load "hero.png"

Gameloop.key_pressed do |key|
  if key == Key::ESCAPE
    Gameloop.quit
  end
end

Gameloop.draw do
  Display.clear(Color.rgb(120, 255, 120))
  
  img.draw Mouse.x, Mouse.y
  
  Graphics.rect 100, 100, 32, 32, fill:true, color:Color.hsv(360.0, 1.0, 0.5)
end

# start game loop.
Gameloop.run
```

This mruby extension require [allegro](http://alleg.sourceforge.net) 5.0.10 or higher(currently testing in Ubuntu 13.10 and Windows 7).

For more information on [Wiki](https://github.com/bggd/mruby-minigame/wiki).

### TO DO

* Add Audio module
* Add Joystick module

