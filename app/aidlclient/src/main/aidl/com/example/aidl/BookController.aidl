// BookController.aidl
package com.example.aidl;

import com.example.aidl.Book;
import com.example.aidl.ICallBack;

interface BookController {
    List<Book> getBookList();
    void addBookInOut(inout Book book);
    void setCallBack(ICallBack callback);
}
