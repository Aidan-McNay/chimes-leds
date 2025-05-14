Appendices
==========================================================================

Appendix A: Permissions
--------------------------------------------------------------------------

* The group approves this report for inclusion on the course website.
* The group approves the video for inclusion on the course YouTube channel.

Appendix B: Source Files
--------------------------------------------------------------------------

An up-to-date listing of our source files can always be found on the
group's `public repository <https://github.com/Aidan-McNay/chimes-leds>`_.
The most recent commit of the repository is additionally included as a
ZIP file below for completeness:

:download:`Download Source Files <files/sources.zip>`

Appendix C: Hardware Designs
--------------------------------------------------------------------------

The group used `KiCad <https://www.kicad.org/>`_ for their PCB design,
with external component symbols/footprints included as necessary from
Digikey's CAD Models (included on the part's page). All of
the design files can be found in the ``pcb`` directory of the source code,
with the schematics of the 3 PCB designs included as PDFs below:

:download:`Control Board Schematic <files/control_schematic.pdf>`

:download:`Sensor Board Schematic <files/sensor_schematic.pdf>`

:download:`Face Board Schematic <files/face_schematic.pdf>`

In addition, a CSV of our Bill of Materials (BOM) can be found here,
with Digikey part numbers and designators on the boards (noting that
the quantities provided are for one face board, but the system uses
four)

:download:`Bill of Materials <files/bom.csv>`

Appendix D: Member Responsibilities
--------------------------------------------------------------------------

* **Aidan**:

  * CMake build system setup
  * Complete hardware/PCB design, bringup, and debugging
  * Small help with utility class implementations