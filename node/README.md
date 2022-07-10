### Build the Bindings
- Run `npm i`
- This will first install and update packages and then run a few commands in sequence: `node-gyp configure`, `node-gyp build`, `npm test`
- The node binaries should be outputed into the build directory
- You can manually trigger unit tests via `npm test` or `npx jest`
- Note that the entry file `index.js` defaults to the pre-build binaries inside the `bin` folder
