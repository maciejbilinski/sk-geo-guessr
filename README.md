# sk-geo-guessr
Simplified version of GeoGuessr as Desktop App

## Message format examples
Allowed:

    state: 1;content: xd ;action:lol; # spaces doesn't matter

    state:-1;action:lol;content:lol; # order doesn't matter

Not allowed:

    state: a;content:b;action:c; # incorrect state

    state:1;content:b;action:c # missing last semicolon

    false:a;content:b;action:c; # missing any part

    state 1;content:b;action:c; # incorrect part format - missing :

    state:1; content:b;action:c; # space after or semicolon

    state:1;action:Perfect string;) ;content:x; # abandoned semicolon in action string

    state:1;action:Perfect string\;) ;content:x; # abandoned semicolon in action string

Preferred:

    state:STATE;action:STR;content:STR;

Where:
- STATE is integer *without spaces* from -1 to 4
- STR is any string without semicolons