#ifndef __COLOR_DEF_H__ 
#define __COLOR_DEF_H__

#ifdef _WIN32
#include <windows.h>
#else
typedef unsigned long COLORREF;
#define RGB(r,g,b)  ((COLORREF)( (r) | ((g)<<8) | ((b)<<16) ))
#endif

/* ³£ÓÃµÄºÃ¿´µÄÑÕÉ« */
#define CLR_MY_BLUE                 RGB(  0, 116, 188)      // À¶É«                
#define CLR_MY_ORANGE               RGB(233,  82,  16)      // ³ÈÉ«
#define CLR_MY_YELLOW               RGB(252, 213,  81)      // »ÆÉ«
#define CLR_MY_RED                  RGB(232, 54,   36)      // ºìÉ«
#define CLR_MY_GREEN                RGB( 93, 213,  16)      // »ÆÂÌÉ«
#define CLR_MY_CYAN                 RGB( 60, 200,  252)      // ÇàÉ«

#define CLR_LIGHTPINK               RGB(255, 182, 193)      // Ç³·Ûºì  
#define CLR_PINK                    RGB(255, 192, 203)      // ·Ûºì  
#define CLR_CRIMSON                 RGB(220,  20,  60)      // ÐÉºì (Éîºì)  
#define CLR_LAVENDERBLUSH           RGB(255, 240, 245)      // µ­×Ïºì  
#define CLR_PALEVIOLETRED           RGB(219, 112, 147)      // Èõ×ÏÂÞÀ¼ºì  
#define CLR_HOTPINK                 RGB(255, 105, 180)      // ÈÈÇéµÄ·Ûºì  
#define CLR_DEEPPINK                RGB(255,  20, 147)      // Éî·Ûºì  
#define CLR_MEDIUMVIOLETRED         RGB(199,  21, 133)      // ÖÐ×ÏÂÞÀ¼ºì  
#define CLR_ORCHID                  RGB(218, 112, 214)      // À¼»¨×Ï  
#define CLR_THISTLE                 RGB(216, 191, 216)      // ¼»  
#define CLR_PLUM                    RGB(221, 160, 221)      // Àî×Ó×Ï  
#define CLR_VIOLET                  RGB(238, 130, 238)      // ×ÏÂÞÀ¼  
#define CLR_MAGENTA                 RGB(255,   0, 255)      // Ñóºì (Æ·ºì Ãµ¹åºì)  
#define CLR_FUCHSIA                 RGB(255,   0, 255)      // µÆÁýº£ÌÄ(×ÏºìÉ«)  
#define CLR_DARKMAGENTA             RGB(139,   0, 139)      // ÉîÑóºì  
#define CLR_PURPLE                  RGB(128,   0, 128)      // ×ÏÉ«  
#define CLR_MEDIUMORCHID            RGB(186,  85, 211)      // ÖÐÀ¼»¨×Ï  
#define CLR_DARKVIOLET              RGB(148,   0, 211)      // °µ×ÏÂÞÀ¼  
#define CLR_DARKORCHID              RGB(153,  50, 204)      // °µÀ¼»¨×Ï  
#define CLR_INDIGO                  RGB( 75,   0, 130)      // µåÇà (×ÏÀ¼É«)  
#define CLR_BLUEVIOLET              RGB(138,  43, 226)      // À¶×ÏÂÞÀ¼  
#define CLR_MEDIUMPURPLE            RGB(147, 112, 219)      // ÖÐ×ÏÉ«  
#define CLR_MEDIUMSLATEBLUE         RGB(123, 104, 238)      // ÖÐ°åÑÒÀ¶  
#define CLR_SLATEBLUE               RGB(106,  90, 205)      // °åÑÒÀ¶  
#define CLR_DARKSLATEBLUE           RGB( 72,  61, 139)      // °µ°åÑÒÀ¶  
#define CLR_LAVENDER                RGB(230, 230, 250)      // Ñ¬ÒÂ²Ýµ­×Ï  
#define CLR_GHOSTWHITE              RGB(248, 248, 255)      // ÓÄÁé°×  
#define CLR_BLUE                    RGB(  0,   0, 255)      // ´¿À¶  
#define CLR_MEDIUMBLUE              RGB(  0,   0, 205)      // ÖÐÀ¶É«  
#define CLR_MIDNIGHTBLUE            RGB( 25,  25, 112)      // ÎçÒ¹À¶  
#define CLR_DARKBLUE                RGB(  0,   0, 139)      // °µÀ¶É«  
#define CLR_NAVY                    RGB(  0,   0, 128)      // º£¾üÀ¶  
#define CLR_ROYALBLUE               RGB( 65, 105, 225)      // »Ê¼ÒÀ¶ (±¦À¶)  
#define CLR_CORNFLOWERBLUE          RGB(100, 149, 237)      // Ê¸³µ¾ÕÀ¶  
#define CLR_LIGHTSTEELBLUE          RGB(176, 196, 222)      // ÁÁ¸ÖÀ¶  
#define CLR_LIGHTSLATEGRAY          RGB(119, 136, 153)      // ÁÁÊ¯°å»Ò  
#define CLR_SLATEGRAY               RGB(112, 128, 144)      // Ê¯°å»Ò  
#define CLR_DODGERBLUE              RGB( 30, 144, 255)      // µÀÆæÀ¶  
#define CLR_ALICEBLUE               RGB(240, 248, 255)      // °®ÀöË¿À¶  
#define CLR_STEELBLUE               RGB( 70, 130, 180)      // ¸ÖÀ¶ (ÌúÇà)  
#define CLR_LIGHTSKYBLUE            RGB(135, 206, 250)      // ÁÁÌìÀ¶É«  
#define CLR_SKYBLUE                 RGB(135, 206, 235)      // ÌìÀ¶É«  
#define CLR_DEEPSKYBLUE             RGB(  0, 191, 255)      // ÉîÌìÀ¶  
#define CLR_LIGHTBLUE               RGB(173, 216, 230)      // ÁÁÀ¶  
#define CLR_POWDERBLUE              RGB(176, 224, 230)      // »ðÒ©Çà  
#define CLR_CADETBLUE               RGB( 95, 158, 160)      // ¾ü·þÀ¶  
#define CLR_AZURE                   RGB(240, 255, 255)      // ÎµÀ¶É«  
#define CLR_LIGHTCYAN               RGB(224, 255, 255)      // µ­ÇàÉ«  
#define CLR_PALETURQUOISE           RGB(175, 238, 238)      // ÈõÂÌ±¦Ê¯  
#define CLR_CYAN                    RGB(  0, 255, 255)      // ÇàÉ«  
#define CLR_AQUA                    RGB(  0, 255, 255)      // Ë®É«  
#define CLR_DARKTURQUOISE           RGB(  0, 206, 209)      // °µÂÌ±¦Ê¯  
#define CLR_DARKSLATEGRAY           RGB( 47,  79,  79)      // °µÊ¯°å»Ò  
#define CLR_DARKCYAN                RGB(  0, 139, 139)      // °µÇàÉ«  
#define CLR_TEAL                    RGB(  0, 128, 128)      // Ë®Ñ¼É«  
#define CLR_MEDIUMTURQUOISE         RGB( 72, 209, 204)      // ÖÐÂÌ±¦Ê¯  
#define CLR_LIGHTSEAGREEN           RGB( 32, 178, 170)      // Ç³º£ÑóÂÌ  
#define CLR_TURQUOISE               RGB( 64, 224, 208)      // ÂÌ±¦Ê¯  
#define CLR_AQUAMARINE              RGB(127, 255, 212)      // ±¦Ê¯±ÌÂÌ  
#define CLR_MEDIUMAQUAMARINE        RGB(102, 205, 170)      // ÖÐ±¦Ê¯±ÌÂÌ  
#define CLR_MEDIUMSPRINGGREEN       RGB(  0, 250, 154)      // ÖÐ´ºÂÌÉ«  
#define CLR_MINTCREAM               RGB(245, 255, 250)      // ±¡ºÉÄÌÓÍ  
#define CLR_SPRINGGREEN             RGB(  0, 255, 127)      // ´ºÂÌÉ«  
#define CLR_MEDIUMSEAGREEN          RGB( 60, 179, 113)      // ÖÐº£ÑóÂÌ  
#define CLR_SEAGREEN                RGB( 46, 139,  87)      // º£ÑóÂÌ  
#define CLR_HONEYDEW                RGB(240, 255, 240)      // ÃÛ¹ÏÉ«  
#define CLR_LIGHTGREEN              RGB(144, 238, 144)      // µ­ÂÌÉ«  
#define CLR_PALEGREEN               RGB(152, 251, 152)      // ÈõÂÌÉ«  
#define CLR_DARKSEAGREEN            RGB(143, 188, 143)      // °µº£ÑóÂÌ  
#define CLR_LIMEGREEN               RGB( 50, 205,  50)      // ÉÁ¹âÉîÂÌ  
#define CLR_LIME                    RGB(  0, 255,   0)      // ÉÁ¹âÂÌ  
#define CLR_FORESTGREEN             RGB( 34, 139,  34)      // É­ÁÖÂÌ  
#define CLR_GREEN                   RGB(  0, 128,   0)      // ´¿ÂÌ  
#define CLR_DARKGREEN               RGB(  0, 100,   0)      // °µÂÌÉ«  
#define CLR_CHARTREUSE              RGB(127, 255,   0)      // ²éÌØ¾ÆÂÌ (»ÆÂÌÉ«)  
#define CLR_LAWNGREEN               RGB(124, 252,   0)      // ²ÝÆºÂÌ  
#define CLR_GREENYELLOW             RGB(173, 255,  47)      // ÂÌ»ÆÉ«  
#define CLR_DARKOLIVEGREEN          RGB( 85, 107,  47)      // °µéÏé­ÂÌ  
#define CLR_YELLOWGREEN             RGB(154, 205,  50)      // »ÆÂÌÉ«  
#define CLR_OLIVEDRAB               RGB(107, 142,  35)      // éÏé­ºÖÉ«  
#define CLR_BEIGE                   RGB(245, 245, 220)      // Ã×É«(»Ò×ØÉ«)  
#define CLR_LIGHTGOLDENRODYELLOW    RGB(250, 250, 210)      // ÁÁ¾Õ»Æ  
#define CLR_IVORY                   RGB(255, 255, 240)      // ÏóÑÀ  
#define CLR_LIGHTYELLOW             RGB(255, 255, 224)      // Ç³»ÆÉ«  
#define CLR_YELLOW                  RGB(255, 255,   0)      // ´¿»Æ  
#define CLR_OLIVE                   RGB(128, 128,   0)      // éÏé­  
#define CLR_DARKKHAKI               RGB(189, 183, 107)      // Éî¿¨ß´²¼  
#define CLR_LEMONCHIFFON            RGB(255, 250, 205)      // ÄûÃÊ³ñ  
#define CLR_PALEGOLDENROD           RGB(238, 232, 170)      // »Ò¾Õ»Æ  
#define CLR_KHAKI                   RGB(240, 230, 140)      // ¿¨ß´²¼  
#define CLR_GOLD                    RGB(255, 215,   0)      // ½ðÉ«  
#define CLR_CORNSILK                RGB(255, 248, 220)      // ÓñÃ×Ë¿É«  
#define CLR_GOLDENROD               RGB(218, 165,  32)      // ½ð¾Õ»Æ  
#define CLR_DARKGOLDENROD           RGB(184, 134,  11)      // °µ½ð¾Õ»Æ  
#define CLR_FLORALWHITE             RGB(255, 250, 240)      // »¨µÄ°×É«  
#define CLR_OLDLACE                 RGB(253, 245, 230)      // ¾ÉÀÙË¿  
#define CLR_WHEAT                   RGB(245, 222, 179)      // Ð¡ÂóÉ«  
#define CLR_MOCCASIN                RGB(255, 228, 181)      // Â¹Æ¤Ñ¥  
#define CLR_ORANGE                  RGB(255, 165,   0)      // ³ÈÉ«  
#define CLR_PAPAYAWHIP              RGB(255, 239, 213)      // ·¬Ä¾¹Ï  
#define CLR_BLANCHEDALMOND          RGB(255, 235, 205)      // ·¢°×µÄÐÓÈÊÉ«  
#define CLR_NAVAJOWHITE             RGB(255, 222, 173)      // ÍÁÖø°×  
#define CLR_ANTIQUEWHITE            RGB(250, 235, 215)      // ¹Å¶­°×  
#define CLR_TAN                     RGB(210, 180, 140)      // ²èÉ«  
#define CLR_BURLYWOOD               RGB(222, 184, 135)      // Ó²Ä¾É«  
#define CLR_BISQUE                  RGB(255, 228, 196)      // ÌÕÅ÷»Æ  
#define CLR_DARKORANGE              RGB(255, 140,   0)      // Éî³ÈÉ«  
#define CLR_LINEN                   RGB(250, 240, 230)      // ÑÇÂé²¼  
#define CLR_PERU                    RGB(205, 133,  63)      // ÃØÂ³  
#define CLR_PEACHPUFF               RGB(255, 218, 185)      // ÌÒÈâÉ«  
#define CLR_SANDYBROWN              RGB(244, 164,  96)      // É³×ØÉ«  
#define CLR_CHOCOLATE               RGB(210, 105,  30)      // ÇÉ¿ËÁ¦  
#define CLR_SADDLEBROWN             RGB(139,  69,  19)      // Âí°°×ØÉ«  
#define CLR_SEASHELL                RGB(255, 245, 238)      // º£±´¿Ç  
#define CLR_SIENNA                  RGB(160,  82,  45)      // »ÆÍÁô÷É«  
#define CLR_LIGHTSALMON             RGB(255, 160, 122)      // Ç³öÙÓãÈâÉ«  
#define CLR_CORAL                   RGB(255, 127,  80)      // Éºº÷  
#define CLR_ORANGERED               RGB(255,  69,   0)      // ³ÈºìÉ«  
#define CLR_DARKSALMON              RGB(233, 150, 122)      // ÉîÏÊÈâ(öÙÓã)É«  
#define CLR_TOMATO                  RGB(255,  99,  71)      // ·¬ÇÑºì  
#define CLR_MISTYROSE               RGB(255, 228, 225)      // ±¡ÎíÃµ¹å  
#define CLR_SALMON                  RGB(250, 128, 114)      // ÏÊÈâ(öÙÓã)É«  
#define CLR_SNOW                    RGB(255, 250, 250)      // Ñ©  
#define CLR_LIGHTCORAL              RGB(240, 128, 128)      // µ­Éºº÷É«  
#define CLR_ROSYBROWN               RGB(188, 143, 143)      // Ãµ¹å×ØÉ«  
#define CLR_INDIANRED               RGB(205,  92,  92)      // Ó¡¶Èºì  
#define CLR_RED                     RGB(255,   0,   0)      // ´¿ºì  
#define CLR_BROWN                   RGB(165,  42,  42)      // ×ØÉ«  
#define CLR_FIREBRICK               RGB(178,  34,  34)      // ÄÍ»ð×©  
#define CLR_DARKRED                 RGB(139,   0,   0)      // ÉîºìÉ«  
#define CLR_MAROON                  RGB(128,   0,   0)      // ÀõÉ«  
#define CLR_WHITE                   RGB(255, 255, 255)      // ´¿°×  
#define CLR_WHITESMOKE              RGB(245, 245, 245)      // °×ÑÌ  
#define CLR_GAINSBORO               RGB(220, 220, 220)      // ¸ýË¹²©ÂÞ»ÒÉ«  
#define CLR_LIGHTGREY               RGB(211, 211, 211)      // Ç³»ÒÉ«  
#define CLR_SILVER                  RGB(192, 192, 192)      // Òø»ÒÉ«  
#define CLR_DARKGRAY                RGB(169, 169, 169)      // Éî»ÒÉ«  
#define CLR_GRAY                    RGB(186, 183, 186)      // »ÒÉ«(*)  
#define CLR_DIMGRAY                 RGB(105, 105, 105)      // °µµ­µÄ»ÒÉ«  
#define CLR_BLACK                   RGB(  0,   0,   0)      // ´¿ºÚ

static CvScalar cvGetColor(COLORREF colorRef)
{
	uchar r,g,b;
	CvScalar color;
	r = colorRef & 0xFF;
	colorRef = colorRef >> 8;
	g = colorRef & 0xFF;
	colorRef = colorRef >> 8;
	b = colorRef & 0xFF;

	color = cvScalar(b,g,r);
	return color;
}

#endif /* __COLOR_DEF_H__ */