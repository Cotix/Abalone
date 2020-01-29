# Abalone board game AI < name pending >

This repo contains a work in progress computer player for Abalone. In my very narrow research, there were only a few online implementations. With ABA-PRO being the most notable one, and the one consitently referenced in papers. However, ABA-PRO is almost 20 years old, and while being decent (searches up to 10ply), I think I can do better. The aim of this project is to create the fastest abalone AI online, and having it opensource as opposed to the closed sourced alternatives like ABA-PRO.


## Current state
Right now work is spend on optimizing the tree search and improving the heuristic. Functionality to actually use this computer player (i.e. have a functioning interface, some kind of decent player api) is not an immediate priority. As such comparing performance is impossible for now. Although ofcourse I do welcome pullrequests.


## Installation

There is no instalation. Compile and run. I recommend compiling with as much optimalization flags you can find. I found the following to be the most useful:
```
g++ main.cpp Game.cpp -O3 -Ofast -march=native -flto 
./a.out
```

## Usage
See main.cpp on how to call the computer player to evaluate positions. Right now usage is limited.


## Contributing
Pull requests are welcome. 

## License
[MIT](https://choosealicense.com/licenses/mit/)
