/**
 * Copyright (c) 2000-2005 Liferay, LLC. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package com.liferay.portal.struts;

import com.liferay.portal.auth.PrincipalException;
import com.liferay.portal.model.Portlet;
import com.liferay.portal.model.User;
import com.liferay.portal.service.spring.PortletServiceUtil;
import com.liferay.portal.service.spring.RoleLocalServiceUtil;
import com.liferay.portal.util.Constants;
import com.liferay.portal.util.PortalUtil;
import com.liferay.portal.util.PropsUtil;
import com.liferay.portal.util.WebKeys;
import com.liferay.portlet.ActionRequestImpl;
import com.liferay.portlet.ActionResponseImpl;
import com.liferay.portlet.PortletConfigImpl;
import com.liferay.portlet.PortletRequestDispatcherImpl;
import com.liferay.portlet.PortletURLImpl;
import com.liferay.portlet.RenderRequestImpl;
import com.liferay.portlet.RenderResponseImpl;
import com.liferay.util.StringPool;
import com.liferay.util.Validator;

import java.io.IOException;

import java.lang.reflect.Constructor;

import javax.portlet.ActionRequest;
import javax.portlet.ActionResponse;
import javax.portlet.PortletException;
import javax.portlet.RenderRequest;
import javax.portlet.RenderResponse;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.struts.Globals;
import org.apache.struts.action.Action;
import org.apache.struts.action.ActionErrors;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;
import org.apache.struts.action.ActionServlet;
import org.apache.struts.config.ForwardConfig;
import org.apache.struts.config.ModuleConfig;
import org.apache.struts.tiles.TilesRequestProcessor;

/**
 * <a href="PortletRequestProcessor.java.html"><b><i>View Source</i></b></a>
 *
 * @author  Brian Wing Shun Chan
 * @version $Revision: 1.29 $
 *
 */
public class PortletRequestProcessor extends TilesRequestProcessor {

	public static PortletRequestProcessor getInstance(
		ActionServlet servlet, ModuleConfig config) throws ServletException {

		try {
			String className = PropsUtil.get(
				PropsUtil.STRUTS_PORTLET_REQUEST_PROCESSOR);

			Class clazz = Class.forName(className);

			Constructor constructor = clazz.getConstructor(
				new Class[] {
					ActionServlet.class, ModuleConfig.class
				}
			);

			PortletRequestProcessor portletReqProcessor =
				(PortletRequestProcessor)constructor.newInstance(
					new Object[] {
						servlet, config
					}
				);

			return portletReqProcessor;
		}
		catch (Exception e) {
			_log.error(e);

			return new PortletRequestProcessor(servlet, config);
		}
	}

	public PortletRequestProcessor(ActionServlet servlet, ModuleConfig config)
		throws ServletException {

		init(servlet, config);
	}

	public void process(RenderRequest req, RenderResponse res)
		throws IOException, ServletException {

		RenderRequestImpl reqImpl = (RenderRequestImpl)req;
		RenderResponseImpl resImpl = (RenderResponseImpl)res;

		HttpServletRequest httpReq = reqImpl.getHttpServletRequest();
		HttpServletResponse httpRes = resImpl.getHttpServletResponse();

		process(httpReq, httpRes);
	}

	public void process(ActionRequest req, ActionResponse res, String path)
		throws IOException, ServletException {

		ActionRequestImpl reqImpl = (ActionRequestImpl)req;
		ActionResponseImpl resImpl = (ActionResponseImpl)res;

		HttpServletRequest httpReq = reqImpl.getHttpServletRequest();
		HttpServletResponse httpRes = resImpl.getHttpServletResponse();

		ActionMapping mapping = processMapping(httpReq, httpRes, path);
		if (mapping == null) {
			return;
		}

		if (!processRoles(httpReq, httpRes, mapping, true)) {
			return;
		}

		ActionForm form = processActionForm(httpReq, httpRes, mapping);
		processPopulate(httpReq, httpRes, form, mapping);
		if (!processValidateAction(httpReq, httpRes, form, mapping)) {
			return;
		}

		PortletAction action =
			(PortletAction)processActionCreate(httpReq, httpRes, mapping);
		if (action == null) {
			return;
		}

		PortletConfigImpl portletConfig =
			(PortletConfigImpl)req.getAttribute(WebKeys.JAVAX_PORTLET_CONFIG);

		try {
			action.processAction(mapping, form, portletConfig, req, res);
		}
		catch (Exception e) {
			String exceptionId =
				WebKeys.PORTLET_STRUTS_EXCEPTION + StringPool.PERIOD +
					portletConfig.getPortletId();

			req.setAttribute(exceptionId, e);
		}

		String forward =
			(String)req.getAttribute(WebKeys.PORTLET_STRUTS_FORWARD);

		if (forward != null) {
			String queryString = StringPool.BLANK;

			int pos = forward.indexOf("?");
			if (pos != -1) {
				queryString = forward.substring(pos + 1, forward.length());
				forward = forward.substring(0, pos);
			}

			ActionForward actionForward = mapping.findForward(forward);

			if ((actionForward != null) && (actionForward.getRedirect())) {
				if (forward.startsWith("/")) {
					PortletURLImpl forwardURL =
						(PortletURLImpl)resImpl.createRenderURL();

					forwardURL.setParameter("struts_action", forward);

					StrutsURLEncoder.setParameters(forwardURL, queryString);

					forward = forwardURL.toString();
				}

				res.sendRedirect(forward);
			}
		}
	}

	protected ActionForm processActionForm(
		HttpServletRequest req, HttpServletResponse res,
		ActionMapping mapping) {

		ActionForm form = super.processActionForm(req, res, mapping);

		if (form instanceof InitializableActionForm) {
			InitializableActionForm initForm = (InitializableActionForm)form;

			initForm.init(req, res, mapping);
		}

		return form;
	}

	protected ActionForward processActionPerform(
			HttpServletRequest req, HttpServletResponse res, Action action,
			ActionForm form, ActionMapping mapping)
		throws IOException, ServletException {

		PortletConfigImpl portletConfig =
			(PortletConfigImpl)req.getAttribute(WebKeys.JAVAX_PORTLET_CONFIG);

		String exceptionId =
			WebKeys.PORTLET_STRUTS_EXCEPTION + StringPool.PERIOD +
				portletConfig.getPortletId();

		Exception e = (Exception)req.getAttribute(exceptionId);

		if (e != null) {
			return processException(req, res, e, form, mapping);
		}
		else {
			return super.processActionPerform(req, res, action, form, mapping);
		}
    }

	protected void processForwardConfig(
			HttpServletRequest req, HttpServletResponse res,
			ForwardConfig forward)
		throws IOException, ServletException {

		if (forward == null) {
			_log.error("Forward does not exist");
		}

		super.processForwardConfig(req, res, forward);
	}

	public ActionMapping processMapping(
			HttpServletRequest req, HttpServletResponse res, String path)
		throws IOException {

		if (path == null) {
			return null;
		}

		return super.processMapping(req, res, path);
	}

	protected boolean processRoles(
			HttpServletRequest req, HttpServletResponse res,
			ActionMapping mapping)
		throws IOException, ServletException {

		return processRoles(req, res, mapping, false);
	}

	protected boolean processRoles(
			HttpServletRequest req, HttpServletResponse res,
			ActionMapping mapping, boolean action)
		throws IOException, ServletException {

		User user = null;

		try {
			user = PortalUtil.getUser(req);
		}
		catch (Exception e) {
		}

		String path = mapping.getPath();

		if (user != null) {
			try {
				PortletConfigImpl portletConfig =
					(PortletConfigImpl)req.getAttribute(
						WebKeys.JAVAX_PORTLET_CONFIG);

				Portlet portlet = PortletServiceUtil.getPortletById(
					user.getCompanyId(), portletConfig.getPortletId());

				if (portlet != null) {
					String strutsPath = path.substring(
						1, path.lastIndexOf(StringPool.SLASH));

					if (!strutsPath.equals(portlet.getStrutsPath())) {
						throw new PrincipalException();
					}
					else if (portlet.isActive()) {
						if (!RoleLocalServiceUtil.hasRoles(
								user.getUserId(), portlet.getRolesArray())) {

							throw new PrincipalException();
						}
					}
					else if (!portlet.isActive()) {
						ForwardConfig forwardConfig =
							mapping.findForward(_PATH_PORTAL_PORTLET_INACTIVE);

						if (!action) {
							processForwardConfig(req, res, forwardConfig);
						}

						return false;
					}
				}
				else {
					return false;
				}
			}
			catch (Exception e) {
				ForwardConfig forwardConfig =
					mapping.findForward(_PATH_PORTAL_PORTLET_ACCESS_DENIED);

				if (!action) {
					processForwardConfig(req, res, forwardConfig);
				}

				return false;
			}
		}

		return true;
	}

	protected boolean processValidateAction(
			HttpServletRequest req, HttpServletResponse res, ActionForm form,
			ActionMapping mapping)
		throws IOException, ServletException {

		if (form == null) {
			return true;
		}

		if (req.getAttribute(Globals.CANCEL_KEY) != null) {
			return true;
		}

		if (!mapping.getValidate()) {
			return true;
		}

		ActionErrors errors = form.validate(mapping, req);
		if ((errors == null) || errors.isEmpty()) {
			return true;
		}

		if (form.getMultipartRequestHandler() != null) {
			form.getMultipartRequestHandler().rollback();
		}

		String input = mapping.getInput();
		if (input == null) {
			_log.error("Validation failed but no input form is available");

			return false;
		}

		req.setAttribute(Globals.ERROR_KEY, errors);

		// Struts normally calls internalModuleRelativeForward which breaks
		// if called inside processAction

		req.setAttribute(WebKeys.PORTLET_STRUTS_FORWARD, input);

		return false;
	}

	protected void doForward(
			String uri, HttpServletRequest req, HttpServletResponse res)
		throws IOException, ServletException {

		doInclude(uri, req, res);
	}

	protected void doInclude(
			String uri, HttpServletRequest req, HttpServletResponse res)
		throws IOException, ServletException {

		PortletConfigImpl portletConfig =
			(PortletConfigImpl)req.getAttribute(WebKeys.JAVAX_PORTLET_CONFIG);

		RenderRequest renderRequest =
			(RenderRequest)req.getAttribute(WebKeys.JAVAX_PORTLET_REQUEST);

		RenderResponse renderResponse =
			(RenderResponse)req.getAttribute(WebKeys.JAVAX_PORTLET_RESPONSE);

		PortletRequestDispatcherImpl prd = (PortletRequestDispatcherImpl)
			portletConfig.getPortletContext().getRequestDispatcher(
				Constants.TEXT_HTML_DIR + uri);

		try {
			if (prd == null) {
				_log.error(uri + " is not a valid include");
			}
			else {
				prd.include(renderRequest, renderResponse, true);
			}
		}
		catch (PortletException pe) {
			Throwable cause = pe.getCause();

			if (cause instanceof ServletException) {
				throw (ServletException)cause;
			}
			else {
				cause.printStackTrace();
			}
		}
	}

	protected HttpServletRequest processMultipart(HttpServletRequest req) {

		// Disable Struts from automatically wrapping a multipart request

		return req;
	}

	protected String processPath(
			HttpServletRequest req, HttpServletResponse res)
		throws IOException {

		String path = req.getParameter("struts_action");

		if (Validator.isNull(path)) {
			path = (String)req.getAttribute(WebKeys.PORTLET_STRUTS_ACTION);
		}

		if (path == null) {
			PortletConfigImpl portletConfig =
				(PortletConfigImpl)req.getAttribute(
					WebKeys.JAVAX_PORTLET_CONFIG);

			_log.error(
				portletConfig.getPortletName() +
					" does not have any paths specified");
		}

		return path;
	}

	private static final Log _log =
		LogFactory.getLog(PortletRequestProcessor.class);

	private static String _PATH_PORTAL_PORTLET_ACCESS_DENIED =
		"/portal/portlet_access_denied";
	private static String _PATH_PORTAL_PORTLET_INACTIVE =
		"/portal/portlet_inactive";

}