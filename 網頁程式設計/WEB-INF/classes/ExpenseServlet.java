package com.expense.servlet;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

@WebServlet("/ExpenseServlet")
public class ExpenseServlet extends HttpServlet {
    
    protected void doPost(HttpServletRequest request, HttpServletResponse response) 
            throws ServletException, IOException {
        
        request.setCharacterEncoding("UTF-8");
        response.setCharacterEncoding("UTF-8");
        
        String action = request.getParameter("action");
        HttpSession session = request.getSession();
        
        @SuppressWarnings("unchecked")
        List<ExpenseRecord> records = (List<ExpenseRecord>) session.getAttribute("records");
        if (records == null) {
            records = new ArrayList<>();
        }
        
        if ("add".equals(action)) {
            // 新增記帳
            String type = request.getParameter("type");
            String amount = request.getParameter("amount");
            String date = request.getParameter("date");
            String category = request.getParameter("category");
            String note = request.getParameter("note");
            
            ExpenseRecord record = new ExpenseRecord(date, type, category, 
                                                     Double.parseDouble(amount), note);
            records.add(record);
            
            session.setAttribute("records", records);
            request.setAttribute("message", "✓ 已新增記帳資料");
        } 
        else if ("delete".equals(action)) {
            // 刪除記帳
            int index = Integer.parseInt(request.getParameter("index"));
            if (index >= 0 && index < records.size()) {
                records.remove(index);
                session.setAttribute("records", records);
                request.setAttribute("message", "✓ 已刪除記帳資料");
            }
        }
        
        session.setAttribute("records", records);
        request.getRequestDispatcher("index.jsp").forward(request, response);
    }
    
    protected void doGet(HttpServletRequest request, HttpServletResponse response) 
            throws ServletException, IOException {
        doPost(request, response);
    }
}
