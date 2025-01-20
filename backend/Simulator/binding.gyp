{
  "targets": [{
    "target_name": "simulator",
    "cflags!": [ "-fno-exceptions" ],
    "cflags_cc!": [ "-fno-exceptions" ],
    "sources": [ "app.cpp" ],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
      "<!(node -e \"console.log(require('node-addon-api').include_dir)\")",
      "<!(node -e \"console.log(require.resolve('node-addon-api').replace('/napi.h', ''))\")",
      "<!(node -e \"console.log(process.env.NODE_PATH || '/usr/local/include/node')\")"
    ],
    'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    'xcode_settings': {
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      'CLANG_CXX_LIBRARY': 'libc++',
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
      'OTHER_CPLUSPLUSFLAGS': ['-std=c++17', '-stdlib=libc++'],
      'OTHER_LDFLAGS': ['-stdlib=libc++']
    }
  }]
}