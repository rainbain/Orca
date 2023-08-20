# Config

## system.json
The file contains all the system settings, like the CPU fan speed graph.
### Setting up a thermal graph.
A thermal graph is part of the `thermal` object and is stored in the `graph` object. It contains objects with the naming scheme of `pt0, pt1, pt2...` Each point contains a `temp` for the maximum temperature this point supports in degress C. As well as the fan speed as a percent.

Points must follow the naming scheme of `pt[X]`. They must start at zero and skip no values. They must be ordered from lowest temperature to greatest temperature.

To make sure your thermal graph was phased correctly, you can check the log for any phrasing errors, as well as a message displaying how many points it phrased.