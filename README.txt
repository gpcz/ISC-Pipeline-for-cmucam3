ISC Pipeline
by Greg Czerniak
------------

This is an independent study project I wrote during my undergrad at the
University of Michgian - Dearborn for their Intelligent Systems Club.  It's an
image processing library for the CMUcam3, written in C89.  It has been tested
extensively with Valgrind to have few/no memory leaks as those are fatal in
this embedded environment, and it was designed to use as little memory as
possible.

Due to the extremely modest hardware specs of this system, this system can only
perform the most basic of image processing operators.  The CMUcam3 does not even
have enough onboard RAM to store an entire image at full resolution, resorting
instead to storing scanlines in a FIFO queue.  Therefore, I didn't implement
any operators that require the entire image to be in memory.

It needs work, but I was borrowing a CMUcam3 for the project and I don't have
one anymore.  As a result, I can't really add anything to this project and I'm
posting it publicly in case someone wants to pick it up from here.

Some of the known issues:
- I never got it to store files on an SD card, which may have been due to a bug
  or possibly because I didn't have any fully-compatible SD card models while
  testing.
- It currently uses malloc/free when creating new lines.  This overuse of 
  dynamic memory allocation could probably be fixed with some kind of static
  memory pool design.

LICENSE: Some of this code is derived from the original CMUcam3 code, so I will
distribute this code with the same license they use -- the Apache License,
Version 2.0 from 2004 (included in LICENSE.txt).
