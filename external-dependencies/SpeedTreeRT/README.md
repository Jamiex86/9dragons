# SpeedTreeRT provisional restoration set

Status: **quarantined candidate — not deployment-approved**

This directory deliberately sits outside the active source overlay. Its files
are preserved for static compatibility analysis and a later controlled-build
decision. Nothing here has been executed.

## Files

| File | Origin | SHA-256 |
|---|---|---|
| `SpeedTreeRT.h` | `multiversemmo/MultiversePlatform`, commit `7e1aad33d48b9e47f3db2ca638cb57592336ddb7`, `lib/SpeedTree/include/SpeedTreeRT.h` | `b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543` |
| `SpeedTreeRT.lib` | Same repository and commit, `lib/SpeedTree/lib/Release/SpeedTreeRT.lib` | `bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f` |
| `SpeedTreeRT.dll` | User-supplied reference-client runtime | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` |

Repository source:

`https://github.com/multiversemmo/MultiversePlatform/tree/7e1aad33d48b9e47f3db2ca638cb57592336ddb7/lib/SpeedTree`

## Technical evidence

- `SpeedTreeRT.lib` is a 32-bit Microsoft COFF import library naming
  `SpeedTreeRT.dll`.
- The library's 146 unique SpeedTree imports match all 146 exports of the
  supplied DLL exactly.
- The supplied DLL is PE32 x86, uses Visual C++ 6-era runtime imports and is
  timestamped 18 January 2006.
- The header contains the later ABI declarations encoded by the supplied DLL,
  including:

```cpp
SetNumWindMatrices(int nNumMatrices);
GetLeafGeometry(SGeometry& sGeometry, short sOverrideLodValue = -1);
Get360BillboardGeometry(SGeometry& sGeometry, unsigned long ulBitVector);
```

## Remaining gates

Before moving these files into an active build dependency path:

1. establish lawful use and redistribution status;
2. trace the Emperor of Dragons SpeedTree authorization mechanism without
   borrowing another product's key or registry state;
3. complete the wrapper call, structure-layout and macro audit;
4. identify the reference client's `.spt` asset generation and expected
   behaviour;
5. record the approved placement and configuration change in the controlled
   build plan.

Do not mix this DLL with the older Metin2 2004 header/import library.

The completed static wrapper assessment is recorded in
`../../SPEEDTREE-WRAPPER-FIT-AUDIT.md`. It found 42/42 called methods and 31/31
directly accessed public fields in the candidate header.
