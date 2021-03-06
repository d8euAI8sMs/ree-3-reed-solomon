#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

#include <reed-solomon/api.h>

static int die(std::wstring msg) { std::wcout << msg << std::endl; return -1; }
static int usage() { return die(L"usage: <encode|noise|decode> <gv> <input> <output> [<ec> <freq>]"); }
static int log(errc_t c)
{
    if (c == errc::errc_invalid_argument) return die(L"arg error");
    return 0;
}

int encode(word_t gv, buf_t in, std::wstring out)
{
    buf_t buf { 0, nullptr };

    errc_t r;
    if (r = rs_encode(in, gv, buf)) return log(r);

    std::ofstream s_out(out, std::ios::out | std::ios::binary);
    s_out.write((const char *) buf.buf, buf.len);
    s_out.close();

    rs_free(buf.buf);

    return 0;
}

int decode(word_t gv, buf_t in, std::wstring out)
{
    buf_t buf { 0, nullptr };

    errc_t r;
    if (r = rs_decode(in, gv, buf)) return log(r);

    std::ofstream s_out(out, std::ios::out | std::ios::binary);
    s_out.write((const char *) buf.buf, buf.len);
    s_out.close();

    rs_free(buf.buf);

    return 0;
}

int noise(word_t gv, buf_t in, int ec, float freq, std::wstring out)
{
    errc_t r;
    if (r = rs_noise(in, gv, (size_t)ec, freq)) return log(r);

    std::ofstream s_out(out, std::ios::out | std::ios::binary);
    s_out.write((const char *) in.buf, in.len);
    s_out.close();

    return 0;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 5 && argc != 7) return usage();

    word_t gv; swscanf_s(argv[2], L"%hu", &gv);

    std::wstring in = argv[3], out = argv[4];

    std::ifstream s_in(in, std::ios::ate | std::ios::binary);
    if (!s_in.is_open()) return die(L"cannot open input file");

    std::vector < byte_t > data(s_in.tellg());

    buf_t buf { (int)data.size(), data.data() };

    s_in.seekg(0, std::ios::beg);
    s_in.read((char*)buf.buf, buf.len);

    s_in.close();

    std::wstring verb = argv[1];

    if (verb == L"encode")
        return encode(gv, buf, out);
    else if (verb == L"decode")
        return decode(gv, buf, out);
    else if (verb == L"noise")
    {
        if (argc != 7) return usage();
        int ec; swscanf_s(argv[5], L"%d", &ec);
        float freq; swscanf_s(argv[6], L"%f", &freq);
        if (ec < 0) return die(L"ec < 0");
        if (freq < 0 || freq > 1) return die(L"freq < 0 | freq > 1");
        return noise(gv, buf, ec, freq, out);
    }
    else return usage();
}
