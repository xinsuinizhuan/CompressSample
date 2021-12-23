/*
***************************************************************************************************
* ���������
* ���������Դ�롢�����ƣ�Ϊ��Դ�������ѭMIT��Դ���Э�飨������������MIT���Э�鷢�ͳ�ͻ���Ա�����Ϊ׼����
* �κθ��˻���֯�����Բ������Ƶ�ʹ�á��޸ĸ������
* �κθ��˻���֯��������Դ����Ͷ����Ƶ���ʽ���·ַ���ʹ�ø������
* �κθ��˻���֯�����Բ������Ƶؽ�����������޸ĺ������汾��������ҵ������ҵ����;��
* 
* ����������
* ���߲���ʹ�á��޸ġ��ַ����Լ������κ���ʽ��ʹ�ã���������������޸ĺ������汾���������ĺ�������κη������Ρ�
* ����Ҳ���Ը�����İ�ȫ�ԡ��ȶ��������κα�֤��
* 
* ʹ�á��޸ġ��ַ������ʱ��Ҫ����������������Ĭ���Ѿ�ͬ������������
***************************************************************************************************
*/
#pragma once

template <typename D>
class scope_exit_t
{
public:
	explicit scope_exit_t(D&& f) : func(f) {}
	scope_exit_t(scope_exit_t&& s) : func(s.func) {}

	~scope_exit_t() { func(); }

private:
	// Prohibit construction from lvalues.
	scope_exit_t(D&);

	// Prohibit copying.
	scope_exit_t(const scope_exit_t&);
	scope_exit_t& operator=(const scope_exit_t&);

	// Prohibit new/delete.
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void*) = delete;
	void operator delete[](void*) = delete;

	const D func;
};

struct scope_exit_helper
{
	template <typename D>
	scope_exit_t<D> operator<< (D&& f) {
		return scope_exit_t<D>(std::forward<D>(f));
	}
};

#define SCOPE_EXIT_CAT2(x, y) x##y
#define SCOPE_EXIT_CAT1(x, y) SCOPE_EXIT_CAT2(x, y)
#define SCOPE_EXIT auto SCOPE_EXIT_CAT1(scope_exit_, __COUNTER__) \
	= scope_exit_helper() << [&]
