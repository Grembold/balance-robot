; EAGLE Autorouter Control File
; optimized for single layer routing
; top side has to be realized with bridges
; rename to BOARDNAME.CTL and start Autorouter

[Default]

  RoutingGrid     = 25mil

  ; Minimum Distances:

  mdWireWire      = 8mil
  mdWirePad       = 8mil
  mdWireDimension = 0.2mm
  mdWireVia       = 8mil
  mdWireRestrict  = 8mil

  mdViaPad        = 8mil
  mdViaDimension  = 40mil
  mdViaVia        = 8mil
  mdViaRestrict   = 8mil

  ; Trace Parameters:

  tpWireWidth     = 16mil
  tpViaDiameter   = 1.4mm
  tpViaDrill      = 24mil
  tpViaShape      = Round

  ; Preferred Directions:

  PrefDir.1       = |
  PrefDir.2       = 0
  PrefDir.3       = 0
  PrefDir.4       = 0
  PrefDir.5       = 0
  PrefDir.6       = 0
  PrefDir.7       = 0
  PrefDir.8       = 0
  PrefDir.9       = 0
  PrefDir.10      = 0
  PrefDir.11      = 0
  PrefDir.12      = 0
  PrefDir.13      = 0
  PrefDir.14      = 0
  PrefDir.15      = 0
  PrefDir.16      = -

  ; Cost Factors:

  cfVia           =    8
  cfNonPref       =   99
  cfChangeDir     =    4
  cfOrthStep      =    2
  cfDiagStep      =   10
  cfExtdStep      =    0
  cfBonusStep     =    1
  cfMalusStep     =    1
  cfPadImpact     =    4
  cfSmdImpact     =    4
  cfBusImpact     =    0
  cfHugging       =    1
  cfAvoid         =    4

  cfBase.1        =   20
  cfBase.2        =    1
  cfBase.3        =    1
  cfBase.4        =    1
  cfBase.5        =    1
  cfBase.6        =    1
  cfBase.7        =    1
  cfBase.8        =    1
  cfBase.9        =    1
  cfBase.10       =    1
  cfBase.11       =    1
  cfBase.12       =    1
  cfBase.13       =    1
  cfBase.14       =    1
  cfBase.15       =    1
  cfBase.16       =    0

  ; Maximum Number of...:

  mnVias          =   20
  mnSegments      = 9999
  mnExtdSteps     = 9999
  mnRipupLevel    =   10
  mnRipupSteps    =  100
  mnRipupTotal    =  100

[Busses]

  @Route

  cfNonPref       =    0
  cfChangeDir     =    1
  cfDiagStep      =    3
  cfBusImpact     =    4
  cfHugging       =    0
  mnVias          =    0

[Route]

  @Default


[Optimize1]

  @Route

  cfVia           =   99
  cfChangeDir     =    6
  cfDiagStep      =   20
  cfExtdStep      =   30
  cfBase.1        =   30
  mnExtdSteps     =    1
  mnRipupLevel    =    0

[Optimize2]

  @Optimize1

  cfChangeDir     =    8
  cfDiagStep      =   30
  cfHugging       =    0
  cfBase.1        =   50

[Optimize3]

  @Optimize2

  cfChangeDir     =   10
  cfDiagStep      =   40
  cfBase.1        =   99

