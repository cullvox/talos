namespace ts {

template<int width>
Font<width>::Font()
{
    
}

template<int width>
Font<width>::Font()
{

}

template<int width>
Font<width>::~Font()
{

}

template<int width>
Result Font<width>::loadFromFile(const char* pPath)
{
    _font = sft_loadfile(pPath);
    if (!_font) return Result::eOutOfMemoryError;
}

template<int width>
Result Font<width>::loadFromMemory(uint32_t memorySize, const uint8_t* memory)
{
    
}

template<int width>
Result Font<width>::render(uint32_t unicode)
{

}

template<int width>
const BitmapInterface& Font<width>::getBitmap() const
{

}

} /* namespace ts */