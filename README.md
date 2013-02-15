This is a buffer based led matrix library for Arduino that has cascadable led matrix boards using Max7219. It is very fast because all display updates are done in memory buffer and only written to hardware when necessary. It is designed to aim at game development using led matrix.

The hardware design is based on Max7219 and can be purchased from oMJKZZ on eBay (http://cgi.ebay.com/ws/eBayISAPI.dll?ViewItem&item=281055201039&ssPageName=STRK:MESE:IT) or any other compatible boards. 

Features:

  Max7219 initialization
  Max7219 intensity setup
  Max7219 shutdonw
  
  Blanking out whole display
  Scroll display horizontally
  Scroll display vertically
  
  getPixel value
  setPixel value
  Line drawing
  Circle drawing
  Rectangle drawing
  
  Sprite animation support
  
  Hardware update that will physically dump the content of display buffer into hardware.

