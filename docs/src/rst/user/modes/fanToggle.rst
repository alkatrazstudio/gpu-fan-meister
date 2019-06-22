Fan toggle
==========

The "Fan toggle" mode allows you to turn the video card fans on and off according to configurable rules. The speed of the switched on fans is controlled by the video card itself.

.. note:: The video card can set the fan speed to zero, but the fans will still be considered to be turned on. In other words, "turned on fan" here means a fan whose speed is controlled by the video card, and "turned off fan" is a fan that does not rotate and the video card does not control it.

The two rules by which the fans will be switched off are combined by logical OR. That is, if both of these rules are checked, the fans will be disabled when any of these conditions are met.

The rules work with an accuracy of one second.
