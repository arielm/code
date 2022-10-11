# code

Source code for some of the examples featured in my [blog](https://arielm.github.io/cross-blog).

### Instructions

1. Following [these instructions](https://arielm.github.io/cross-blog/2022/10/06/hello-world.html) for running on macOS or Linux, or [these instructions](https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html) for running in the browser.

2. Clone this repository with:
   ```
   git clone https://github.com/arielm/code
   ```

3. Add `CODE_PATH` variable:
   ```
   export CODE_PATH=path/to/code
   ```

4. Pick a sketch, e.g.
   ```
   cd $CODE_PATH/sketches/Drawing2dText
   ```

5. Run for your platform of choice:
   ```
   RUN_TEST -DPLATFORM=???
   ```
   Supported platform identifiers: `osx`, `linux`, `emscripten`
