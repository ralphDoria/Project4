#include "FileDataSource.h"

CFileDataSource::CFileDataSource(const std::string &filename){
    DFile.open(filename);
    if(DFile.good()){
        DFile.peek();
    }
}

bool CFileDataSource::End() const noexcept{
    return DFile.eof();
}

bool CFileDataSource::Get(char &ch) noexcept{
    if(!DFile.good()){
        return false;   // LCOV_EXCL_LINE
    }
    ch = DFile.get();
    if(ch != EOF){
        DFile.peek();
    }
    return ch != EOF;
}

bool CFileDataSource::Peek(char &ch) noexcept{
    if(!DFile.good()){
        return false;   // LCOV_EXCL_LINE
    }
    ch = DFile.peek();
    return ch != EOF;
}

bool CFileDataSource::Read(std::vector<char> &buf, std::size_t count) noexcept{
    if(!DFile.good()){
        return false;   // LCOV_EXCL_LINE
    }
    char TempCh;
    buf.clear();
    buf.reserve(count);
    while(buf.size() < count && Get(TempCh)){
        buf.push_back(TempCh);
    }
    return !buf.empty();
}
