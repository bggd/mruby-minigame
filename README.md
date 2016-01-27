mruby-minigame
==============

mruby-minigame is a 2D game framework for rapid prototyping.

example:

``` ruby
include MiniGame

Display.create 640, 480

img = Image.load 'hero.png'

GameLoop.key_pressed do |key|
  if key == Key::ESCAPE
    GameLoop.quit
  end
end

GameLoop.draw do
  Display.clear [32, 64, 128] # or Display.clear(Color.rgb(32, 64, 128))
  
  img.draw Mouse.x, Mouse.y
  
  Graphics.rect(100, 100, 32, 32,
                fill:true,
                color:Color.hsv(360.0, 1.0, 0.5))
end

# start game loop.
GameLoop.run
```

This mruby extension(mrbgem) require [Allegro](http://liballeg.org/) **5.1.12** or higher(currently tested in Windows 10 with visual studio 2015).

For more information at [Wiki](https://github.com/bggd/mruby-minigame/wiki).

