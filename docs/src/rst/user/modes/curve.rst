Curve
=====

The "Curve" mode allows you to graphically define the relationship between the video card temperature and the required fan speed.

The following actions can be performed on the interactive chart:
    * Add a new point to the curve by double-clicking on the free space in the chart.
    * Delete a point by double-clicking on it.
    * Move the point with the mouse.

.. note:: The chart points are shown only when you point the mouse at them.

.. note:: The border points of the chart can be moved vertically.

The red dot shows the current temperature and fan speed.

.. note:: The actual fan speed may not match exactly what is shown on the graph. This depends on the video card. For example, if the video card does not support low fan speeds, the actual fan speed may be 30% when set to 5%.


.. _curve-hysteresis:

Hysteresis
----------

Settings in the "Hysteresis"[#hysteresis]_ section specify the lag of temperature, which is used to calculate the fan speed, from the actual current temperature of the video card. This avoids the "contact bounce"[#contact_bounce]_ effect. Let's assume that for 60°C the fan speed is set to 0%, and for 61°C it is set to 15%. If the temperature of the graphics card fluctuates between 60°C and 61°C, the fans will be constantly switching on and off. Hysteresis allows you to not change the speed of the fans immediately, but only if the temperature has not returned to the previous value for some time.

For convenience, let's label the current video card temperature as |T0|, and the temperature used to calculate the required fan speed according to the graph as |T1|. For example, using the data from the previous example, if |T0| = 61°C and |T1| = 60°C, the fan speed will be 0% as it is calculated based on |T1|. Initially, these temperatures are equal. If |T0| changes, |T1| stars moving towards |T0|, but only after a specified time or if the difference between |T0| and |T1| is greater than the specified value.

|project| allows to set the following hysteresis parameters:

.. _curve-maxIncTempDiff:

* **Max inc. temp. diff.** - when the temperature |T0| increases, |T1| may lag behind |T0| by a specified number of degrees Celsius maximum.

.. _curve-incDelay:

* **Inc. delay** - if |T0| > |T1|, then |T1| starts moving towards |T0| only after the specified number of seconds.

.. _curve-maxDecTempDiff:

* **Max inc. temp. diff.** - when the temperature |T0| decreases, |T1| may lag behind |T0| by a specified number of degrees Celsius maximum.

.. _curve-decDelay:

* **Dec. delay** - if |T0| < |T1|, then |T1| starts moving towards |T0| only after the specified number of seconds.


.. rubric:: Notes

.. [#hysteresis] The "Hysteresis" article in Wikipedia: `<https://en.wikipedia.org/wiki/Hysteresis>`_.
.. [#contact_bounce] The "Contact bounce" article in Wikipedia: `<https://en.wikipedia.org/wiki/Switch#Contact_bounce>`_.


.. |T0| replace:: T\ :sub:`0`
.. |T1| replace:: T\ :sub:`1`
