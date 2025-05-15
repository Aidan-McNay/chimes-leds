# chimes-leds
A system for controlling McGraw Tower's clock face LEDs

## Documentation

Documentation for our system is located [here](https://aidan-mcnay.github.io/chimes-leds/)

Our documentation is built using Sphinx, and is
formatted as [reStructured Text](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html).
To build the documentation locally, first install Sphinx,
as well as our additional Sphinx modules:

```bash
pip install sphinx sphinx_rtd_theme sphinxcontrib-youtube
```

Then, you can build the output files in a `build`
directory with Sphinx, with the top-level HTML
located at `build/index.html`

```bash
sphinx-build docs build
```

This process is automated to update the Github page
on a push
