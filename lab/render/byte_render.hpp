#include <array>
#include <algorithm>
#include <stl/types.hpp>
#include <crt/math.hpp>
#include "byte_render_data.hpp"

class FColor
{
private:
    union { u8 arr[4]; u32 full; };
    inline static int rIndx, gIndx, bIndx, aIndx;

public:
    //set color pallete
    static void SetCChannelIndx(int r, int g, int b, int a) {
        rIndx = r; gIndx = g; bIndx = b; aIndx = a;
    }

    static void SetCChannelIndx(int* rgba) {
        rIndx = rgba[0]; gIndx = rgba[1];
        bIndx = rgba[2]; aIndx = rgba[3];
    }

    static auto GetCChannelIndx() {
        return std::array<int, 4>{ rIndx, gIndx, bIndx, aIndx };
    }

    //construct color
    FColor(u8 R, u8 G, u8 B, u8 A = 255) {
        arr[rIndx] = R; arr[gIndx] = G;
        arr[bIndx] = B; arr[aIndx] = A;
    }

    FColor(const u32& rawPixel) {
        full = rawPixel;
    }

    //get pixel colors
    template<typename T = u8>
    auto RGBA() {
        return std::array{ (T)arr[rIndx], (T)arr[gIndx], (T)arr[bIndx], (T)arr[aIndx] };
    }

    auto& RawPixel() const {
        return full;
    }

    //get pixel channell
    auto& r() { return *(u8*)&arr[rIndx]; }
    auto& g() { return *(u8*)&arr[gIndx]; }
    auto& b() { return *(u8*)&arr[bIndx]; }
    auto& a() { return *(u8*)&arr[aIndx]; }

    //static colors
    inl static FColor White() {
        return FColor(255, 255, 255);
    }

    inl static FColor Black() {
        return FColor(0, 0, 0);
    }

    inl static FColor Red() {
        return FColor(255, 0, 0);
    }

    inl static FColor Green() {
        return FColor(0, 255, 0);
    }

    inl static FColor Blue() {
        return FColor(0, 0, 255);
    }
};

class ByteRender
{
private:
public:
    int w, h;
    uint32_t* pix_arr;
    bool alpha_sup;
    
    //helpers struct
    struct Bounds {
        int x0, y0, x1, y1;
        int rect_w, rect_h;
    };
    struct CharDesc {
        short srcX, srcY;
        short srcW, srcH;
        short xOff, yOff;
        short xAdv;
    };

    //pixel bounds
    inl bool PosOOB(int& x, int& y, bool clamp = false) {
        bool oob = (x < 0) || (x >= w) || (y < 0) || (y >= h);
        if (oob && clamp) {
            x = std::clamp(x, 0, w - 1);
            y = std::clamp(y, 0, h - 1);
        } return oob;
    }

    inl auto ClampRect(int x0, int y0, int x1, int y1) {
        PosOOB(x0, y0, true);
        PosOOB(x1, y1, true);
        return Bounds{ x0, y0, x1, y1, abs(x1 - x0), abs(y1 - y0) };
    }

    //pixel set, get, blend
    FColor ColorBlend(FColor prev, FColor cur)
    {
        //pixels rgba to float
        auto added = cur.RGBA<float>();//{ (float)cur.RGBA[0], (float)cur.RGBA[1], (float)cur.RGBA[2], (float)cur.RGBA[3] / 255.f };
        auto base = prev.RGBA<float>();// { (float)prev.RGBA[0], (float)prev.RGBA[1], (float)prev.RGBA[2], (float)prev.RGBA[3] / 255.f };
        added[3] /= 255.f;
        base[3] /= 255.f;

        //mix colors
        float mix[4];
        mix[3] = 1 - (1 - added[3]) * (1 - base[3]); // alpha
        mix[0] = __roundf((added[0] * added[3] / mix[3]) + (base[0] * base[3] * (1 - added[3]) / mix[3])); // red
        mix[1] = __roundf((added[1] * added[3] / mix[3]) + (base[1] * base[3] * (1 - added[3]) / mix[3])); // green
        mix[2] = __roundf((added[2] * added[3] / mix[3]) + (base[2] * base[3] * (1 - added[3]) / mix[3])); // blue

        //set new pixel color
        return FColor(mix[0], mix[1], mix[2], (mix[3] * 255.f));
    }

   
    void SetPixel(int x, int y, FColor color, int blackout = 0)
    {
        if (PosOOB(x, y))
            return;
        
        if (blackout)
        {
            //alpha blend (color + blackout)
            auto a1 = (float)color.a() / 255.f;
            auto a2 = 1.f - ((float)blackout / 255.f);
            auto dstA = (u8)((a1 * a2) * 255.f);
            color.a() = dstA;
            
            //no alpha (skip pix)
            if (!dstA)
                return;
        
            //color.r() = (u8)((float)color.r() * a1);
            //color.g() = (u8)((float)color.g() * a1);
            //color.b() = (u8)((float)color.b() * a1);

            //color blend (bbuf + fbuf)
            auto prevPx = FColor(0);// pix_arr[x + (y * w)]);
            if (!alpha_sup) { prevPx.a() = 255; }
            color = ColorBlend(prevPx, color);

            color.a() = a2 * 255;
        }
        


        //fill pixel
        //if (!alpha_sup) { color.a() = 255; }

        //Line2(x, y, x + 1, y + 1, color);
        //SetPixel2(x, y, color);
        //FillRGB((float)x, (float)y, 1, 1, D3DCOLOR_RGBA(color.r(), color.g(), color.b(), color.a()), g_pd3dDevice);
        pix_arr[x + (y * w)] = color.RawPixel();
    }

    //bresenham
    void plotLineWidth(int x0, int y0, int x1, int y1, float wd, FColor color)
    {                                    /* plot an anti-aliased line of width wd */
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx - dy, e2, x2, y2;                           /* error value e_xy */
        float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

        for (wd = (wd + 1) / 2; ; ) {                                    /* pixel loop */
            SetPixel(x0, y0, color, max(0, 255 * (abs(err - dx + dy) / ed - wd + 1)));
            e2 = err; x2 = x0;
            if (2 * e2 >= -dx) {                                            /* x step */
                for (e2 += dy, y2 = y0; e2 < ed * wd && (y1 != y2 || dx > dy); e2 += dx)
                    SetPixel(x0, y2 += sy, color, max(0, 255 * (abs(e2) / ed - wd + 1)));
                if (x0 == x1) break;
                e2 = err; err -= dy; x0 += sx;
            }
            if (2 * e2 <= dy) {                                             /* y step */
                for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy); e2 += dy)
                    SetPixel(x2 += sx, y0, color, max(0, 255 * (abs(e2) / ed - wd + 1)));
                if (y0 == y1) break;
                err += dx; y0 += sy;
            }
        }
    }

    //text
    bool GetCharDesc(wchar_t Char, CharDesc* Desc)
    {
        int CharID = Char;
        CharDesc* FDBuff = (CharDesc*)&FontDesc;

        if ((CharID >= 32) && (CharID <= 126)) { //latin
            *Desc = FDBuff[0 + (CharID - 32)];
            return true;
        }

        else if ((CharID >= 1040) && (CharID <= 1103)) { //cyrillic
            *Desc = FDBuff[96 + (CharID - 1040)];
            return true;
        }

        else if (CharID == 1105) { //�
            *Desc = FDBuff[160 + (CharID - 1105)];
            return true;
        }

        else if (CharID == 1025) { //�
            *Desc = FDBuff[95 + (CharID - 1025)];
            return true;
        }

        //no char
        return false;
    }

    Vector2 GetTextSize(const wchar_t* String, CharDesc* DescArray, int* DescSize)
    {
        wchar_t WChar;
        int DescArrayOffset = 0;
        Vector2 TextSize = { 0.f, 0.f };
        for (int i = 0; (WChar = String[i]); ++i)
        {
            //get char description
            if (GetCharDesc(WChar, &DescArray[DescArrayOffset]))
            {
                //get char size
                CharDesc CDesc = DescArray[DescArrayOffset++];
                float YSize = float(CDesc.yOff + CDesc.srcH);
                TextSize.x += (float)CDesc.xAdv;
                TextSize.y = max(YSize, TextSize.y);
            }
        }

        //save size & ret text size
        *DescSize = DescArrayOffset;
        return TextSize;
    }

    //setup frame
    template <typename T>
    inl void Setup(T pixelsArr, int width, int height, int pitch = 0) {
        h = height; pix_arr = (u32*)pixelsArr;
        w = pitch ? pitch / 4 : width;
    }

    inl void Clear() {
        __memzero(pix_arr, w * h * 4);
    }

    //line
    inl void Line(const Vector2& Start, const Vector2& End, const FColor& Color = FColor::White(), float Thick = 1.f) {
        //Line2(Start.x, Start.y, End.x, End.y, Color);
        auto bnd = ClampRect(Start.x, Start.y, End.x, End.y);
        plotLineWidth(bnd.x0, bnd.y0, bnd.x1, bnd.y1, Thick, Color);
    }

    void DrawCircle(float x, float y, float radius, const FColor& color = FColor::White(), float thickness = 2.f) {
        int numOfPoints = 100;
        float pi2 = 6.28318530718;
        float fSize = numOfPoints;
        float alpha = 1 / fSize * pi2;
        // matrix coefficients
        const float cosA = cos(alpha);
        const float sinA = sin(alpha);
        // initial values
        float curX = radius;
        float curY = 0;
        for (size_t i = 0; i < numOfPoints; ++i) {
            Line({ curX + x, curY + y }, { curX + x + thickness, curY + y + thickness }, color);
            // recurrence formula
            float ncurX = cosA * curX - sinA * curY;
            curY = sinA * curX + cosA * curY;
            curX = ncurX;
        }
    }

    //rect
    inl void Rectangle(const Vector2& Start, const Vector2& Size, const FColor& Color = FColor::White(), float Thick = 1.f) 
    {
        //TODO: FIX
        FillRectangle({ Start.x, Start.y }, { Size.x, 1 }, Color);
        FillRectangle({ Start.x, Start.y }, { 1, Size.y }, Color);
        FillRectangle({ Start.x + Size.x, Start.y }, { 1, Size.y }, Color);
        FillRectangle({ Start.x, Start.y + Size.y }, { Size.x + 1, 1 }, Color);
    }

    inl void FillRectangle(const Vector2& Start, const Vector2& Size, const FColor& Color) {
        for (int x = 0; x <= static_cast<int>(Size.x); x++)
        {
            for (int y = 0; y <= static_cast<int>(Size.y); y++)
            {
                SetPixel(static_cast<int>(Start.x + x), static_cast<int>(Start.y + y), Color);
            }
        }
    }

    //render text
    Vector2 String(Vector2 Start, const wchar_t* String, bool Center = false, FColor Color = FColor(255, 255, 255))
    {
        //get char desc & string size (in px)
        if (String == nullptr) return Vector2{ 0.f, 0.f };
        int CharDescSize = 0; CharDesc CharDesc[256];
        Vector2 TextSize = GetTextSize(String, CharDesc, &CharDescSize);
        if (Center) { Start -= (TextSize / 2.f); }

        auto drawTexture = [&](const Vector2& Start, u8* Texture, const Vector2& TexOff, const Vector2& RectSize) {
            for (int y = 0; y < RectSize.y; y++) {
                for (int x = 0; x < RectSize.x; x++) {
                    //int off1 = (Start.x + x) + ((Start.y + y) * w);
                    int off2 = ((TexOff.x + x) + ((TexOff.y + y) * 170)) * 4;

                    //if (!FColor(Texture[off2]).RGBA[0] && !FColor(Texture[off2]).RGBA[1] && !FColor(Texture[off2]).RGBA[2])
                    {
                        //
                        //auto g = FColor::GetCChannelIndx();
                        //FColor::SetCChannelIndx(0, 1, 2, 3);
                        //auto gg = FColor(Texture[off2]);
                        //auto colorAvg =/* 1.f -*/ ((float)((gg.RGBA<float>()[0] + gg.RGBA<float>()[1] + gg.RGBA<float>()[2]) / 3) / 255.f);
                        //auto A = FColor(Texture[off2]).RGBA<float>()[3];
                        //FColor::SetCChannelIndx(g.data());
                        //auto newg = FColor((float)Color.RGBA<float>()[0] * colorAvg, (float)Color.RGBA<float>()[1] * colorAvg, (float)Color.RGBA<float>()[2] * colorAvg, A);
                        auto clr = /*ColorBlend(*/FColor(Texture[off2], Texture[off2 + 1], Texture[off2 + 2], Texture[off2 + 3]);// , Color);
                        
                        //if(Texture[off2 + 3] == 255){
                        //
                        //    SetPixel(Start.x + x, Start.y + y, Color, 0);
                        //}
                         if (Texture[off2 + 3] > 0) {
                            //alpha_sup = 0;
                            SetPixel(Start.x + x, Start.y + y, clr, 255 - Texture[off2 + 3]);
                            //alpha_sup = 1;
                        }

                        //printf("%d\n\n", FColor(Texture[off2]).RGBA[3]);
                        //Arr[off1] = Texture[off2];
                    }
                    //else 
                    {
                        //SetPixel(Start.x + x, Start.y + y, ColorBlend(Texture[off2], Color), 0);
                    }
                }
            }
        };

        //draw chars
        for (int i = 0; i < CharDescSize; ++i)
        {
            //add vertex to draw list
            //auto FillChar = RenderToolKit::AddDrawCmd(4, 6);
            float srcX = (float)CharDesc[i].srcX, srcY = (float)CharDesc[i].srcY,
                srcW = (float)CharDesc[i].srcW, srcH = (float)CharDesc[i].srcH,
                xOff = (float)CharDesc[i].xOff, yOff = (float)CharDesc[i].yOff,
                xAdv = (float)CharDesc[i].xAdv;



            drawTexture({ Start.x + (xOff /** MP*/), Start.y + (yOff /** MP*/) }, (u8*)&FontBytes[0], { srcX, srcY }, { srcW, srcH });



            //add vertexes
            //auto _VtxWritePtr = FillChar.VBuffer;

            //_VtxWritePtr[0] = { { Start.x + (xOff * MP), //top left
            //	Start.y + (yOff * MP) }, { srcX, srcY }, Color.DXColor };
            //
            //_VtxWritePtr[1] = { { Start.x + ((xOff + srcW) * MP), //upper right
            //	Start.y + (yOff * MP) }, { srcX + srcW, srcY }, Color.DXColor };
            //
            //_VtxWritePtr[2] = { { Start.x + (xOff * MP), //bottom left
            //	Start.y + ((yOff + srcH) * MP) }, { srcX, srcY + srcH }, Color.DXColor };
            //
            //_VtxWritePtr[3] = { { Start.x + ((xOff + srcW) * MP), //bottom right
            //	Start.y + ((yOff + srcH) * MP) }, { srcX + srcW, srcY + srcH }, Color.DXColor };

            //add indexes
            //FillChar.IBuffer[0] = 0; FillChar.IBuffer[1] = 1; FillChar.IBuffer[2] = 2;
            //FillChar.IBuffer[3] = 3; FillChar.IBuffer[4] = 1; FillChar.IBuffer[5] = 2;

            //set char spacing
            Start.x += (xAdv /** MP*/);
        }

        return TextSize;
    }
};
