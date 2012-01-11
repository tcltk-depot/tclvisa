@erase tclvisa.wixobj
@candle tclvisa.wxs -ext WixUIExtension
@light tclvisa.wixobj -ext WixUIExtension -out tclvisa-0.1.1-x86.msi
