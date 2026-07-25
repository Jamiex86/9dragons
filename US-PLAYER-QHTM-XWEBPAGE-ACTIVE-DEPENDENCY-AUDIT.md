# US Normal Player QHTM and XWebPage Dependency Audit

Date: 2026-07-25  
Scope: `US_Release|Win32`, static inspection only. No compilation, linking,
DLL loading, or binary execution.

## Conclusion

QHTM and XWebPage are **active normal US player dependencies**, not stale
project entries.

- QHTM is part of the local help/HTML-to-texture system and is initialized
  unconditionally during client initialization.
- XWebPage embeds the Windows/OLE web browser for the item mall, event pages,
  in-game web board, and the US Rubicon advertising window.
- The supplied reference `NineDragons.exe` imports both DLLs.
- The recovered import libraries and supplied DLLs are exact binary pairs.

Technical dependency confidence: **99–100%**.

## QHTM

`XControl/XHTMLWrapper.cpp` defines `_XDEF_ENABLEQHTMWRAPPER` locally and calls
`QHTM_Initialize`, `QHTM_Uninitialize`, `QHTM_GetHTMLHeight`, and
`QHTM_RenderHTML`. Client initialization calls
`_XHTMLWrapper::InitializeQHTMLibrary()` unconditionally.

The wrapper loads local HTML such as `HelpSystem/error.html`, calculates its
height, draws it into a 32-bit DIB, and transfers it to a Direct3D texture.
An absent normally imported `QHTM.dll` prevents process startup at the Windows
loader stage.

| File | SHA-256 |
|---|---|
| `Library/QHTM/QHTM.h` | `3ca9196b83b38299dad870febd767b9e8f1de3fbc3441e651b8658b342e68cd0` |
| `Library/QHTM/QHTM.lib` | `b3d9d5badcceb19e90fa8ced0366558de87f998f561f865b863a6f85bdac2f67` |
| `Library/QHTM/QHTM.dll` | `9e622b880d9afc7076a8c008272528bfea21ec3d2e95b354795246cd48b29e53` |

The import library and DLL match all 16 used ordinals exactly. The reference
executable imports `QHTM.dll` by ordinal.

## XWebPage

`XWebPage.h` declares `EmbedBrowserObject`, `UnEmbedBrowserObject`,
`DisplayHTMLPage`, `DisplayHTMLStr`, `DoPageAction`, and `ResizeBrowser`.
Source creates child windows, embeds the browser, navigates it, resizes it, and
detaches it.

Normal US Release actively compiles the Rubicon browser because it defines
`_ACCLAIM_RUBICONADSYSTEM`. Historical URLs include:

```text
http://www.acclaim.com/ads/9d_rubicon_728_90.html
http://9dragons.acclaim.com/store/store.jsp
```

These are obsolete service assumptions, not viable future endpoints.

| File | SHA-256 |
|---|---|
| `Library/CWebPage/XWebPage.h` | `6f59e531b766b0619d22c1d294cdadb8e4ede55e334aa13eb1c463f491c5e4c3` |
| `Library/CWebPage/XWebPage.lib` | `b7d761dc7449438fbc445c302900148826ee02ebe58bf6e7b81df2f14921b4c2` |
| Supplied `XWebPage.dll` | `23f5f9b55b64971481e3e8495e212c417e454f028a28900844b03d53be6a7562` |

All six stdcall imports match the DLL exactly. The reference executable imports
`XWebPage.dll`.

## Disposition

| Dependency | Current criticality | Future option |
|---|---|---|
| QHTM | Required for link, startup, and faithful local help rendering | Replace/disable only as a deliberate UI change |
| XWebPage | Required by current US source and loader; supports active Rubicon/web UI | Can later be stripped with every browser call/window path, or replaced |

For the preservation baseline, retain both import libraries. QHTM's exact DLL
is already staged. The exact XWebPage DLL is supplied externally and awaits
controlled runtime-manifest placement. Endpoint replacement/removal is a later
product decision, not a missing-dependency repair.
